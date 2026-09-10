# Data-Driven Design (Section 26)

Everything a designer should be able to rebalance without touching C++ or recompiling lives
in one of these three places:

| Value                                                        | Location |
|-----------------------------------------------------------------|----------|
| Objective value bands, interaction duration, noise on complete  | `Content/Data/DT_Objectives.csv` -> `DT_Objectives` DataTable (`FSHObjectiveDataRow`) |
| AI sight/hearing radius, suspicion thresholds, chase duration, movement speed per archetype | `Content/Data/DT_ResidentArchetypes.csv` -> `DT_ResidentArchetypes` DataTable (`FSHResidentArchetypeDataRow`) |
| Match duration, goal money, detection limit                     | `Config/DefaultGame.ini` under `[/Script/SuburbanHeist.SHGameMode]`, or per-instance on a `BP_GameMode` child |
| Player movement speed (walk/sprint/crouch), noise-per-action amounts | `EditDefaultsOnly` properties on `BP_PlayerCharacter` (inherited from `ASHPlayerCharacter`) |
| Noise decay rate                                                 | `EditDefaultsOnly` properties on `BP_NoiseManager` |
| Fictional chase-attack range/cooldown/accuracy/telegraph          | Per-node properties on the `Resident Attack (Fictional)` BT task instance in `BT_Resident` |

None of these require editing a `.h`/`.cpp` file. Every corresponding C++ class reads the
`EditAnywhere`/`EditDefaultsOnly`/`Config` values at runtime rather than hardcoding them, so a
designer can rebalance the whole game from the Editor's Details panels, the DataTable editor,
and `DefaultGame.ini`.

## Adding a new objective tier
1. Add a row to `DT_Objectives.csv` (or directly in the imported `DT_Objectives` DataTable
   asset - CSV is just the initial seed, the asset is the source of truth after import).
2. Reference the new row name from any `BP_ObjectivePoint` instance's `DataRowName`.

## Adding a new resident archetype
1. Add a row to `DT_ResidentArchetypes.csv` with a new `Archetype` value - note this also
   requires adding the new entry to the `EResidentArchetype` UENUM in `SHTypes.h` (this one
   step *does* need a C++ change + recompile, since Blueprint-only data can't extend a C++
   enum; everything else about the archetype's behavior stays data-only).
2. Create `BP_Resident_<NewName>` per `Docs/BLUEPRINT_CONSTRUCTION.md`.
