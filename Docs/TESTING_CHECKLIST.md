# Testing Checklist

Mirrors the Section 31 acceptance test. Run with 2-4 Play-In-Editor clients
(listen server + 1-3 clients) once `Docs/SETUP_INSTRUCTIONS.md` is complete.

- [ ] **1-2. Join & spawn**: all clients spawn together at the cul-de-sac `PlayerStart`
      cluster; `ASHGameMode::BeginCountdown` runs before match start.
- [ ] **3. 20-minute timer visible**: `WBP_HUD` top-center shows `20:00` counting down once
      `EGamePhase::Active` begins; confirm it's server-driven (`SetTimer 60` via console
      updates instantly on all clients, proving replication rather than a client-local timer).
- [ ] **4-5. Enter houses & interact**: walk into a house, aim at an `ASHObjectivePoint`,
      confirm the `[E] ...` prompt appears (`Docs/UI_SPEC.md`), hold E, confirm the progress
      bar fills and the objective completes at the configured duration.
- [ ] **6-7. Generate & see team money**: completing an objective increases
      `TEAM CASH` on **every** client's HUD, not just the one who interacted - proves
      `ASHLootManager::Server_AwardMoney` -> `ASHGameState::TeamMoney` replication.
- [ ] **8-9. Generate noise & attract AI**: sprint or jump near a `BP_Resident_*`; confirm
      the Noise meter (if enabled in HUD) rises and the resident transitions
      Idle -> Suspicious -> Investigating (visible via `Debug_SetAIPaused(false)`/logging
      `CurrentState`, or a temporary on-screen debug string bound to
      `ASHResidentCharacter::OnStateChanged`).
- [ ] **10. Investigation behavior**: resident visibly moves toward `LastHeardLocation` while
      Investigating (via `UBTTask_MoveToSuspicionLocation`).
- [ ] **11-12. Detection & unique-count increment**: let a resident reach `SuspicionLevel >=
      DetectionThreshold` (or run `TriggerDetection` for a fast test); confirm
      `DETECTION` HUD counter increments, and that the **same** resident detecting a second
      player does **not** increment it again (Section 5 worked example) while a
      **different** resident detecting anyone **does**.
- [ ] **13. Chase**: after Detected, resident enters Chase (`ChaseSpeed`), and if
      `Tuning.bCanAttack`, the fictional `Resident Attack` BT task telegraphs then resolves;
      confirm a player who breaks `Range` during the telegraph avoids the hit.
- [ ] **14-15. Third detection -> instant game over**: trigger 3 unique detections (or
      `TriggerDetection` x3); confirm `EGamePhase::PoliceGameOver` fires immediately on all
      clients and gameplay stops (no further objective interaction possible).
- [ ] **16-17. Reach $10,000 -> Victory**: `GiveMoney 10000` (or organic play); confirm
      `EGamePhase::Victory` fires the moment the threshold is crossed, even with time
      remaining (`ASHLootManager::Server_AwardMoney` -> `ASHGameMode::Server_CheckWinCondition`).
- [ ] **17b. Timer expires under goal -> Defeat**: `SetTimer 1` with `TeamMoney < GoalMoney`;
      confirm `EGamePhase::Defeat` fires at 0:00.
- [ ] **18. Replay**: from any end screen, confirm a new match can start (server travel or a
      simple `ResetDetection` + `SetTimer` + phase reset flow) without restarting the process.

## Multiplayer-specific checks
- [ ] Kill the server's connection to one client mid-match; confirm remaining clients keep
      playing (standard UE session handling, no custom work needed beyond default replication).
- [ ] Confirm `USHCheatManager` exec commands run on a client actually mutate server state
      (not just that client's local view) - e.g. `GiveMoney` from Client 2's console updates
      Client 1's HUD too.
- [ ] Confirm a client with `USHGameInstance::bMicrophoneDetectionEnabled = false` still
      generates world noise from movement/interactions (Section 6 fallback requirement).

## Performance sanity (Section 27)
- [ ] Confirm `ASHResidentCharacter` shows `Tick: Disabled` in the Editor's Actor Tick
      statistics (logic runs via `UBTService_UpdateSuspicion` instead).
- [ ] Confirm `USHStaminaComponent`/`USHInteractionComponent` component tick disables itself
      when idle (visible via `stat game` or by inspecting `IsComponentTickEnabled()` while the
      player stands still and isn't interacting).
