# M_CulDeSac Construction Spec (Section 15)

`Python/generate_culdesac_blockout.py` spawns an immediately-playable greybox: a circular
road plus 10 house/yard blockouts in a ring. This doc describes the full readable layout to
build on top of that greybox with real block-style modular pieces (Section 3/12).

## Layout
- **Central road**: a circular/oval loop, ~50m diameter, flanked by sidewalks.
- **8-12 houses** around the cul-de-sac, each with its own driveway connecting to the road.
- Each house footprint (~8m x 10m) contains, assembled from the modules in
  `Docs/HOUSE_MODULE_SPEC.md`: front door, 1-2 hallways, living room, kitchen, 1-2 bedrooms,
  optionally a garage and a fenced backyard.
- Perimeter dressing: streetlights at regular intervals, 2-3 parked cars (block-style, non-
  interactive), trees/bushes along property lines, low fences between yards (climbable or
  a chokepoint - designer's choice for pacing).

## Required placed actors (in addition to house/yard geometry)
- 4x `PlayerStart` (2-4 players, Section 2) clustered at the cul-de-sac center, all facing
  outward so players immediately see the ring of houses.
- One each of `BP_TeamDetectionManager`, `BP_LootManager`, `BP_NoiseManager`,
  `BP_AudioManager` (Section 25) - placed anywhere; `ASHGameMode::FindOrSpawnManagers`
  will use them if present or spawn a fallback if a designer forgets one.
- `ASHObjectivePoint` instances (via the `BP_ObjectivePoint*` children from
  `Docs/BLUEPRINT_CONSTRUCTION.md`) scattered through the houses - mix of Common/Uncommon/
  Rare/HighRisk rows per house so the risk/reward curve (Section 14) holds even before full
  randomization logic is layered on top.
- `BP_Resident_*` instances - place 1-3 per house depending on archetype mix; vary
  archetypes across the map so players learn "the Sensitive one is the risky house."

## Randomization (Section 23)
For matches to feel different each time without hand-authoring N layouts, add a lightweight
level Blueprint or a `ASHObjectivePlacementVolume` (a simple `AActor` marking valid spawn
points per room) that at `BeginPlay` (server-only) picks a random subset of pre-placed marker
points to actually spawn `ASHObjectivePoint`s at, and randomizes which `BP_Resident_*`
archetype occupies which pre-placed resident spawn marker. This keeps the map's read
(Section 23 "map itself should remain readable") constant while objective/resident placement
varies - exactly the requested balance.

## Readability rules
- Keep house silhouettes visually distinct (roof shape/color) so players can call out
  "the blue house" over voice chat without confusion (Section 1 "team coordination").
- Streetlights and yard fences should never fully block sightlines from the road into a
  yard - players need to gauge risk from outside before committing (Section 14).
