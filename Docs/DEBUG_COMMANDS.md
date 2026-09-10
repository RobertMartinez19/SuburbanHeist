# Debug Console Commands (Section 24)

Implemented as `Exec` functions on `USHCheatManager` (`SHCheatManager.h/.cpp`), gated behind
`USHGameInstance::bDebugModeEnabled` (defaults to `true` outside `UE_BUILD_SHIPPING`). Open
the console with `~` in a Play-In-Editor or standalone session and type:

| Command                        | Effect |
|----------------------------------|--------|
| `GiveMoney 10000`                | Awards the team `Amount` dollars via `ASHGameMode::Debug_GiveMoney` -> `ASHLootManager::Server_AwardMoney` |
| `TriggerDetection`               | Bumps the unique-resident-detected counter by one (fabricated, no real resident required) |
| `ResetDetection`                 | Clears the detection counter back to 0/3 |
| `SpawnResident Calm`             | Spawns an `ASHResidentCharacter` a few meters in front of you with the given archetype name (`Calm`/`Alert`/`Sensitive`/`Patrol`) |
| `CompleteObjective`               | Force-completes the first not-yet-completed `ASHObjectivePoint` found in the level |
| `SetTimer 60`                    | Sets `MatchTimeRemaining` directly |
| `WinMatch`                       | Jumps straight to the Victory phase |
| `LoseMatch`                      | Jumps straight to the Defeat phase |
| `ToggleNoise`                    | Flips `ASHNoiseManager::bNoiseSystemEnabled` - while off, noise events are ignored entirely (no AI hearing stimuli, no meter movement) |
| `ToggleAI`                       | Pauses/resumes every `ASHResidentAIController`'s Behavior Tree logic via `StopLogic`/`RestartLogic` |

All of these are server-authoritative under the hood (the `UCheatManager` only exists on
connections with cheats allowed, and its exec functions route through `ASHGameMode`'s
`Debug_*` methods which run on the server) - safe to leave enabled in a private playtest build
without opening up client-side money/detection exploits.
