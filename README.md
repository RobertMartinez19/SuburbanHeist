# Suburban Heist

A fictional, chaotic 2-4 player co-op party game prototype for Unreal Engine 5.4. A team of
friends has 20 minutes to earn $10,000 by exploring a stylized, low-poly suburban cul-de-sac,
completing abstract fictional objectives, and managing noise/risk before three different
resident NPCs notice them and the police are called. All burglary/weapons/police/money
systems are entirely abstract gameplay mechanics — see **Fictional framing** below.

This repository is Blueprint-first-by-design: every gameplay system is implemented in C++ as
a reusable base class, and the actual game content (Blueprints, materials, the map, Behavior
Trees, UI widgets) is meant to be authored as thin children/instances of that C++ inside the
Unreal Editor. **This environment cannot emit binary `.uasset`/`.umap` files** — see
`Docs/LIMITATIONS.md` for exactly what that means and what's provided instead (construction
specs + Python editor automation).

## Start here
1. `Docs/SETUP_INSTRUCTIONS.md` — compile the C++ module and get a playable greybox running.
2. `Docs/TESTING_CHECKLIST.md` — verify the full 20-minute loop end-to-end (mirrors the
   design brief's acceptance test).
3. `Docs/LIMITATIONS.md` — what's C++/data/docs vs. what needs manual Editor work, and why.

## What's implemented (C++, `Source/SuburbanHeist/`)
- **Match flow**: `ASHGameMode` / `ASHGameState` / `ASHPlayerState` — server-authoritative
  20-minute timer, phases (Lobby/Countdown/Active/Victory/Defeat/PoliceGameOver), win/lose.
- **Team detection**: `ASHTeamDetectionManager` — dedupes detections by resident identity
  exactly per spec (same resident detecting two players counts once; different residents
  each count), triggers instant game-over at 3 unique detections.
- **Noise**: `ASHNoiseManager` (world noise, decay, AI hearing stimuli) +
  `USHMicrophoneNoiseComponent` (optional mic amplitude → noise, no speech recognition,
  toggleable, with an action-based fallback when disabled).
- **Loot/objectives**: `ASHLootManager`, `ASHObjectivePoint` (data-driven value bands,
  abstract hold-to-interact, never real lock-bypass mechanics).
- **Resident AI**: `ASHResidentCharacter` (server-authoritative Idle → Suspicious →
  Investigating → Alert → Detected → Chase → Return state machine) + `ASHResidentAIController`
  (AIPerception sight/hearing) + custom Behavior Tree nodes. Four archetypes
  (Calm/Alert/Sensitive/Patrol) are data-only tuning via `DT_ResidentArchetypes`, not separate
  classes.
- **Fictional chase attack**: `UBTTask_ResidentAttack` — abstract telegraph/cooldown/range/
  accuracy mechanic, no weapon/ammo/ballistics simulation.
- **Player**: `ASHPlayerCharacter` + stamina/interaction/microphone components, Enhanced
  Input–driven, first-person.
- **UI plumbing**: `ASHHUD` / `USHHUDWidgetBase` — push-driven off replicated GameState, no
  polling.
- **Debug**: `USHCheatManager` — `GiveMoney`, `TriggerDetection`, `SpawnResident`, `SetTimer`,
  `ToggleNoise`, `ToggleAI`, etc. (`Docs/DEBUG_COMMANDS.md`).

## What needs Editor work (all specced/scripted, none hand-waved)
- Enhanced Input assets, every `BP_*`/`WBP_*` child class → `Docs/BLUEPRINT_CONSTRUCTION.md`
- `BT_Resident`'s node graph → `Docs/BEHAVIOR_TREE_SPEC.md` (`Python/create_ai_assets.py`
  creates the Blackboard/BehaviorTree assets themselves)
- The cul-de-sac map → `Docs/MAP_CONSTRUCTION.md` + `Docs/HOUSE_MODULE_SPEC.md`
  (`Python/generate_culdesac_blockout.py` gives an immediately-playable greybox)
- Stylized materials → `Docs/MATERIALS_SPEC.md`
- HUD widget layout → `Docs/UI_SPEC.md`

## Data-driven balance
`Content/Data/DT_Objectives.csv` and `Content/Data/DT_ResidentArchetypes.csv` hold every
tunable value (objective payouts/noise/duration, AI perception/suspicion/speed per archetype).
Import via `Python/create_data_tables.py`. Match duration/goal/detection-limit live in
`Config/DefaultGame.ini`. See `Docs/DATA_DRIVEN_DESIGN.md`.

## Fictional framing
Per the design brief, every "heist" mechanic here is an abstract gameplay system: objectives
are generic interact-and-wait/QTE prompts with no real lock-picking or security-bypass logic,
"police" is just a game-over state transition, and the fictional chase attack has no weapon
simulation. Nothing in this codebase teaches or enables real-world burglary, lock-picking, or
security-system bypass.
