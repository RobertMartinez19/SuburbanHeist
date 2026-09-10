# Blueprint Construction Spec

Every Blueprint below is a **thin child of a C++ class** - create with right-click in the
Content Browser > Blueprint Class > (search the C++ parent name) > place in the folder shown.
None of these need graph logic beyond what's listed; the gameplay logic lives in C++ so
designers only tune exposed properties and hook up meshes/widgets/sounds.

## Enhanced Input assets (create first - everything else references them)
Path: `/Game/Input/`
- `IMC_Default` (Input Mapping Context)
- `IA_Move` (Value Type: Axis2D) - bound to IMC_Default: WASD -> 2D axis, gamepad left stick
- `IA_Look` (Axis2D) - Mouse XY, gamepad right stick
- `IA_Jump` (Digital/bool) - Spacebar, gamepad Face Button Bottom
- `IA_Sprint` (Digital/bool) - Left Shift, gamepad Left Thumbstick button
- `IA_Crouch` (Digital/bool) - Left Ctrl / C, gamepad Face Button Right
- `IA_Interact` (Digital/bool) - E, gamepad Face Button Right... (pick a free button; Crouch
  and Interact share layout slots on gamepad by design choice of your control scheme)

## Player
Path: `/Game/Blueprints/Characters/`
- **BP_PlayerCharacter** (parent `ASHPlayerCharacter`)
  - Assign a block-style humanoid Skeletal Mesh to the inherited `Mesh` component (chunky
    proportions, per Section 3). In the mesh component's Component Details, enable
    **Owner No See** is NOT what you want for a full-body first-person mesh - instead set
    **Visibility > Owner No See** on just the *head* sub-mesh/socket if you split head/body,
    or simplest: leave the full body visible (a stylized party game can show your own body -
    this reads fine at the exaggerated proportions Section 3 calls for; only hide it if it
    clips the camera).
  - Set `DefaultMappingContext = IMC_Default`, `MoveAction = IA_Move`, `LookAction = IA_Look`,
    `JumpAction = IA_Jump`, `SprintAction = IA_Sprint`, `CrouchAction = IA_Crouch`,
    `InteractAction = IA_Interact`.
  - Assign `MicrophoneComponent` nothing extra needed - it self-configures from
    `USHGameInstance::bMicrophoneDetectionEnabled`.
  - `HealthComponent` also needs no required setup (defaults to 100 HP, 2s stagger, comedic
    knockback on getting caught by a Resident's chase attack - Section 11/18); tune
    `KnockbackStrength`/`StaggerDurationSeconds` to taste. Bind its `OnStaggered`/`OnRecovered`
    events in `WBP_HUD` if you want a visual "you got bumped!" cue.

## Gameplay Managers (place ONE of each in the map, or let ASHGameMode spawn a fallback)
Path: `/Game/Blueprints/Gameplay/`
- **BP_TeamDetectionManager** (parent `ASHTeamDetectionManager`) - no properties to set;
  `DetectionLimit` is pushed by GameMode at match start.
- **BP_NoiseManager** (parent `ASHNoiseManager`) - tune `DecayPerSecond`/`DecayTickInterval`
  if you want noise to fade faster/slower than the Section 7 example (80 -> 60 -> 35 -> 10 -> 0).
- **BP_LootManager** (parent `ASHLootManager`) - no properties to set.
- **BP_AudioManager** (parent `ASHAudioManager`) - assign placeholder `USoundBase`/`USoundCue`
  assets (or Metasounds) to every `SoundBase` property (Section 21). Leave any unset if you
  truly have no placeholder yet - the Play* functions null-check before playing.

## Interaction & Objectives
Path: `/Game/Blueprints/Objectives/`
- **BP_ObjectivePoint** (parent `ASHObjectivePoint`) - assign a block-style prop mesh to
  `ObjectiveMesh`. Set `Category` (drives the `[E] ...` prompt text), `ObjectiveDataTable =
  DT_Objectives`, `DataRowName` to one of: `Common_Valuable`, `Uncommon_Cache`,
  `Uncommon_Electronics`, `Rare_Safe`, `Collectible_Small`, `HighRisk_Bonus`.
  - Optionally make category-flavored children (`BP_ObjectivePoint_Valuable`,
    `BP_ObjectivePoint_Safe`, ...) that just hardcode `Category`/`DataRowName` and swap the
    mesh, so level designers drag-and-drop rather than configuring every instance by hand.

## Resident AI
Path: `/Game/Blueprints/AI/`
- **BP_Resident_Calm**, **BP_Resident_Alert**, **BP_Resident_Sensitive**,
  **BP_Resident_Patrol** (all parent `ASHResidentCharacter`) - each sets `Archetype` to the
  matching `EResidentArchetype` value and `ArchetypeDataTable = DT_ResidentArchetypes`
  (row name is auto-derived from the enum, e.g. `Calm`). Assign a distinct block-style
  civilian Skeletal Mesh + a simple color-coded material per archetype so players can start
  to read "that one's the twitchy one" purely by silhouette/color (Section 3 exaggeration).
- **BP_ResidentAIController** (parent `ASHResidentAIController`) - set
  `BehaviorTreeAsset = BT_Resident` (see `Docs/BEHAVIOR_TREE_SPEC.md`). Assign this as the
  `AIControllerClass` on each `BP_Resident_*` above.

## UI
Path: `/Game/UI/`
- **WBP_HUD** (parent `USHHUDWidgetBase`) - build the layout per `Docs/UI_SPEC.md` and
  implement the `On*Changed` events inherited from the C++ base.
- Set `ASHHUD::HUDWidgetClass = WBP_HUD` via a **BP_HUD** child of `ASHHUD` (or set it
  directly on `ASHGameMode`'s `HUDClass` if you don't need a BP child at all - `ASHGameMode`
  already defaults `HUDClass` to `ASHHUD` in C++; only create BP_HUD if you want to set
  `HUDWidgetClass` without touching GameMode).

## Game Framework
Path: `/Game/Blueprints/Gameplay/`
- **BP_GameMode** (parent `ASHGameMode`) - set `MatchDurationSeconds`/`GoalMoney`/
  `DetectionLimit` here if you prefer Blueprint-editable defaults over `Config/DefaultGame.ini`.
- **BP_GameState** (parent `ASHGameState`), **BP_PlayerState** (parent `ASHPlayerState`) -
  no required properties; create them only if you want Blueprint-only cosmetic additions.
- Assign `BP_GameMode` as the map's Game Mode Override (World Settings) or as
  `GlobalDefaultGameMode` in `Config/DefaultEngine.ini` (already points at
  `/Script/SuburbanHeist.SHGameMode` - repoint it at `BP_GameMode` once you create it).
