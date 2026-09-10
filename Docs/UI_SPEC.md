# WBP_HUD Layout Spec (Section 20)

Parent class: `USHHUDWidgetBase` (`UI/SHHUDWidgetBase.h`). The C++ base already binds to
`ASHGameState` and `ASHNoiseManager` and fires the `BlueprintImplementableEvent`s below -
build the Canvas Panel layout and implement each event to update the corresponding widget.
No polling needed anywhere; every event is push-driven off replication.

```
┌──────────────────────────────────────────────────────────────────┐
│  TEAM CASH                    20:00                  DETECTION   │
│  $4,250 / $10,000                                       1 / 3     │
│                                                                     │
│                                                                     │
│                         (gameplay viewport)                        │
│                                                                     │
│                                                                     │
│  [Player status icons]                            [Noise meter]   │
│                        [E] SEARCH                                  │
│                     ▓▓▓▓▓▓▓░░░ 70%                                 │
└──────────────────────────────────────────────────────────────────┘
```

| Region          | Widget(s)                                   | Driven by                          |
|------------------|-----------------------------------------------|--------------------------------------|
| Top center       | `TXT_Timer` ("MM:SS")                        | `OnTimeRemainingChanged` -> `FormatTime()` |
| Top left         | `TXT_TeamCash` ("$X,XXX / $10,000")          | `OnTeamCashChanged`                  |
| Top right        | `TXT_Detection` ("N / 3"), flashes red on increment | `OnDetectionChanged`         |
| Bottom center    | `TXT_Prompt` + `PB_InteractProgress`         | `OnInteractionFocusChanged` / `OnInteractionProgressChanged` (bind directly to the local pawn's `USHInteractionComponent` delegates in Event Construct - see note below) |
| Bottom left      | Player status icons (detected/not, per teammate) | `ASHPlayerState::bIsCurrentlyDetected`, read per-teammate via `GetPlayerState()->bIsCurrentlyDetected` on a per-frame-cheap Tick or, better, bind each teammate's PlayerState `OnRep` if you add one |
| Optional overlay | `WBP_NoiseMeter` (0-100 bar, color ramps green->yellow->red across the bands in Section 6) | `OnNoiseLevelChanged` |

**Wiring the interaction prompt from Blueprint:** `USHHUDWidgetBase` doesn't auto-bind to the
pawn's `InteractionComponent` (it can change every time the player respawns/possesses a new
pawn), so in `WBP_HUD`'s Event Construct: get owning player pawn -> get its
`InteractionComponent` -> Bind Event to `OnFocusChanged` -> call `OnInteractionFocusChanged`;
same for `OnInteractProgress` -> `OnInteractionProgressChanged`.

## End-of-match overlays
Bind `OnGamePhaseChanged`:
- `EGamePhase::Victory` -> show `WBP_Victory` (Section 20/22 `OnVictory`)
- `EGamePhase::Defeat` -> show `WBP_Defeat`
- `EGamePhase::PoliceGameOver` -> show `WBP_PoliceGameOver` ("POLICE CALLED" -> "GAME OVER",
  per Section 4's exact beat)

Each of these three widgets is a simple full-screen `WBP_*` with a headline text, the final
team cash total, and a "Return to Lobby"/"Play Again" button wired to your session-restart
flow (Section 31 acceptance test #18 "complete another match").

## Client-side stinger/music hookup (Section 21)
Bind `OnGamePhaseChanged` (every client receives this via `ASHGameState::OnRep_GamePhase`) to
call the local `ASHAudioManager` instance's `PlayVictoryStinger`/`PlayDefeatStinger`/
`PlayPoliceGameOverStinger` - find it once via `GetAllActorsOfClass` in Event Construct and
cache it. This is why `ASHAudioManager` doesn't need any multicast RPCs (see its header
comment) - every client independently reacts to the same replicated phase change.
