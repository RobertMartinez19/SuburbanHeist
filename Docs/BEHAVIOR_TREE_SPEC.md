# BB_Resident / BT_Resident Construction Spec

`Python/create_ai_assets.py` creates both assets and every Blackboard key except
`DetectionState` (see below). This doc covers the one manual key and the full BT node graph.

## Blackboard: BB_Resident

| Key                | Type                          | Notes                                   |
|--------------------|-------------------------------|------------------------------------------|
| TargetActor        | Object (Actor)                | currently perceived/chased player       |
| LastHeardLocation  | Vector                        | last noise stimulus location            |
| SuspicionLevel     | Float                         | 0-100, mirrors `ASHResidentCharacter`   |
| IsInvestigating    | Bool                          | true in Investigating or Alert          |
| HasDetectedPlayer  | Bool                          | true in Detected or Chase               |
| DetectionState     | Enum (`EResidentState`)       | add manually - see below                |
| HomeLocation       | Vector                        | set once in `OnPossess`                 |

**Adding DetectionState manually:** open `BB_Resident` in the editor, add a new key, set
its type to **Enum**, and in the key's details pick **Enum Type = EResidentState** (search
for it - it appears once the C++ module is compiled). Name the key exactly `DetectionState`
to match `SHBlackboardKeys::DetectionState` in `AI/SHAIBlackboardKeys.h`.

## BT_Resident node graph

Root
└─ Service: **Update Suspicion** (`UBTService_UpdateSuspicion`, Interval 0.5s) - always attached
   at the root so it runs regardless of which branch below is active.
└─ Selector "State Router"
   ├─ Sequence "Chase" - Decorator: **Is In State** (`RequiredState = Chase`)
   │  ├─ Task: **Move To Suspicion Location** (`bUseTargetActor = true`, `AcceptableRadius = 150`)
   │  └─ Task: **Resident Attack (Fictional)** (`Range=250, Cooldown=2.5, Accuracy=0.6,
   │     TelegraphSeconds=0.6`) - place this as a **Simple Parallel** alongside the Move task
   │     if you want the Resident to keep closing distance while telegraphing; a plain
   │     Sequence (move, then attempt attack, loop) also reads fine for a party game.
   ├─ Sequence "Investigating/Alert" - Decorator: **Is In State** with an **Or** composite of
   │  `Investigating` and `Alert` (add two `Is In State` decorators in "Or" mode, one per
   │  state, via the decorator's Blackboard-based composite operator, or simply duplicate this
   │  branch once per state if you'd rather keep it explicit and readable)
   │  └─ Task: **Move To Suspicion Location** (`bUseTargetActor = false`, uses
   │     `LastHeardLocation`, `AcceptableRadius = 100`)
   ├─ Sequence "Suspicious" - Decorator: **Is In State** (`RequiredState = Suspicious`)
   │  └─ Task: built-in "Wait" (0.5-1.5s, RandomDeviation) representing the "NPC pauses,
   │     looks toward noise" beat from Section 10 - implement the actual head-turn/VO as a
   │     BlueprintImplementableEvent on `ASHResidentCharacter` bound in a BP child, called
   │     from a small custom task, or simply an Anim Notify keyed off `CurrentState`.
   ├─ Sequence "Return" - Decorator: **Is In State** (`RequiredState = Return`)
   │  └─ Task: **Return Home** (`AcceptableRadius = 80`)
   └─ Sequence "Idle" (default/fallback, no decorator needed - lowest priority)
      └─ Task: built-in **Move To** using an EQS wander point, or a simple custom
        "Wait then pick random point in radius" pair of built-in tasks - this is ordinary
        idle-wander behavior with no Suburban-Heist-specific logic, so the stock Behavior
        Tree "MoveTo" + "Wait" tasks are sufficient here.

Selector priority (top to bottom) matches state priority: Chase > Investigating/Alert >
Suspicious > Return > Idle. Because `ASHResidentCharacter` owns all state transitions
(see `Server_SetState`/`Server_AddSuspicion` in `AI/SHResidentCharacter.cpp`), the BT only
ever *reads* `CurrentState` via `Is In State` decorators - it never decides state itself,
which keeps the tree simple and keeps the authoritative FSM in one place for networking.

## EQS (optional, Section 25 folder)
`/Game/AI/EQS/EQS_ResidentWanderPoint` - a minimal "points around HomeLocation" query
(Generator: Points: Grid, centered on Blackboard `HomeLocation`, radius ~600) filtered by
Navmesh reachability. Wire it into the Idle branch's MoveTo task via a "Run EQS Query" task
if you want the Idle wander to look less robotic than a single Wait/MoveTo loop.
