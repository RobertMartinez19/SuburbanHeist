# Multiplayer Replication Model (Section 18)

## Authority summary

| State                          | Owner                        | Replication mechanism                          |
|----------------------------------|-------------------------------|--------------------------------------------------|
| Match timer, phase, goal/limit   | `ASHGameState`                | `Replicated` + `RepNotify`, ticked 1Hz server-side (`ASHGameMode::Server_TickMatchTimer`) - not per-frame, to keep bandwidth low (Section 27) |
| Team money                       | `ASHGameState::TeamMoney`     | Only ever written by `ASHLootManager::Server_AwardMoney` (server) |
| Unique resident detection count  | `ASHGameState::UniqueResidentsDetected` (mirrored from `ASHTeamDetectionManager`'s server-only dedup set) | Server-only write, `RepNotify` broadcast |
| Per-resident AI state/suspicion  | `ASHResidentCharacter::CurrentState` / `SuspicionLevel` | `Replicated`, server-only write via `Server_SetState`/`Server_AddSuspicion` |
| Objective completion/value       | `ASHObjectivePoint::bCompleted` / `RolledValue` | `Replicated`, server-only write in `Interact_Implementation` |
| Player stamina                   | `USHStaminaComponent::CurrentStamina` | `Replicated`, server-only Tick |
| Player "currently detected" flag | `ASHPlayerState::bIsCurrentlyDetected` | `Replicated`, set by `ASHTeamDetectionManager::Server_ReportDetection` |

**Every** `Interact`, `AddSuspicion`, `SetState`, `AwardMoney`, `ReportDetection`, and
`ReportNoise` entry point begins with `if (!HasAuthority()) return;` - clients can call the
`Server_*` RPCs (via `USHInteractionComponent::Server_RequestCompleteInteraction`,
`ASHPlayerCharacter::Server_EmitNoise`, `USHMicrophoneNoiseComponent::Server_ReportVoiceNoise`)
but never write the replicated state directly. This satisfies Section 18's "do not trust
client-side money or objective completion."

## Why some managers aren't replicated at all
`ASHLootManager` and `ASHAudioManager` have `bReplicates = false`. `ASHLootManager` has no
client-visible state of its own (everything it changes lives on `ASHGameState`, which *is*
replicated) - replicating an empty actor wastes a network object channel. `ASHAudioManager`
deliberately runs identically-but-independently on every machine (see `Docs/UI_SPEC.md`'s
"client-side stinger hookup" note) so it never needs to be told anything by the server.

## Server RPC list

| RPC                                                        | Reliability | Purpose |
|--------------------------------------------------------------|-------------|---------|
| `USHInteractionComponent::Server_RequestCompleteInteraction`| Reliable    | Client claims an interaction finished; server re-validates distance + `CanInteract` before calling `Interact` |
| `ASHPlayerCharacter::Server_EmitNoise`                       | Unreliable  | Jump/sprint/landed noise - fine to drop occasionally, next tick corrects |
| `USHMicrophoneNoiseComponent::Server_ReportVoiceNoise`       | Unreliable  | Same reasoning, plus mic sampling is already lossy |

`Client_NotifyResidentSuspicious`/`Client_NotifyResidentDetected` on `ASHPlayerController` are
the one deliberate **Client** RPC pair - used for a per-player cosmetic "you personally were
just glanced at" cue that doesn't belong on the shared `ASHGameState` (only the player who was
looked at needs it).

## Bandwidth notes (Section 27)
- Match timer replicates once per second, not per tick - clients can count down locally
  between updates for a smooth display without spamming replication.
- `ASHResidentCharacter` has `PrimaryActorTick.bCanEverTick = false` - all suspicion/state
  logic is driven by the Behavior Tree's `UBTService_UpdateSuspicion` (interval-ticked, not
  per-frame) rather than the actor's own Tick.
- `USHStaminaComponent` and `USHInteractionComponent` only enable their component tick while
  actively needed (sprinting/regenerating, or mid-interaction) and disable it otherwise.
