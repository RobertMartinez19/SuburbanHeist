# Modular House Kit Spec (Section 12)

Block-style, low-poly modular kit built on a **400 x 400 x 300 (cm) grid cell** so every
piece snaps together without gaps. Build each as a Static Mesh (or, for a first pass, the
`Python/generate_culdesac_blockout.py` primitives) placed on a `ModularSnap` grid setting.

| Module          | Grid Footprint | Notes                                                        |
|-----------------|----------------|---------------------------------------------------------------|
| Wall (solid)    | 1 x 0.1 x 1    | Chunky, flat-shaded, bright single-color material            |
| Wall (windowed) | 1 x 0.1 x 1    | Simple rectangular cutout, no glass simulation needed         |
| Door frame      | 1 x 0.1 x 1    | Paired with a separate swinging door mesh (Section 21 door SFX hook via `ASHAudioManager::PlayDoorSound`) |
| Floor tile      | 1 x 1 x 0.1    | Reused for every room                                          |
| Hallway segment | 1 x 2 x 1      | Connects rooms                                                 |
| Bedroom shell   | 3 x 3 x 1      | Furnish with low-poly bed/dresser/nightstand props             |
| Kitchen shell   | 3 x 4 x 1      | Counter/fridge/table low-poly props                            |
| Living room shell | 4 x 4 x 1    | Sofa/TV/coffee-table props - good spot for `Rare_Safe`/`HighRisk_Bonus` objectives |
| Garage shell    | 3 x 4 x 1      | Optional; door-to-backyard shortcut for escape routes           |
| Backyard plot   | 4 x 4 x 0      | Fence perimeter, 1-2 decorative props, optional side entrance   |

## Furniture / prop set (low-poly, per Section 3)
Nightstand, dresser, bed, sofa, TV, coffee table, kitchen counter, fridge, dining table,
chairs, bookshelf, safe (visual only - `ASHObjectivePoint` with `Category=SafeObjective`),
electronics cluster (laptop/TV/console - `Category=ElectronicsObjective`), wall art/
collectibles (`Category=Collectible`), a garage workbench (`Category=HiddenCache`).

Each interactable prop is simply a Static Mesh child actor under a `BP_ObjectivePoint*`
instance (Section 12) - the mesh itself has no gameplay logic; all interaction/value/noise
logic lives in `ASHObjectivePoint` (C++).

## Assembly
Because the kit is grid-snapped, a house is just a sequence of module placements; use Modeling
Mode's "Instance/Array" tools or a simple Construction Script on a `BP_HouseAssembler` actor
that arranges wall/floor modules from a designer-editable `TArray<FHouseModuleEntry>` if you
want fully data-driven house layouts later (out of scope for the prototype - hand-placing
8-12 houses from this kit is a reasonable afternoon's work).
