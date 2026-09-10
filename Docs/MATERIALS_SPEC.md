# Material Spec (Section 3)

Stylized, flat-shaded, high-saturation, minimal texture complexity. Every material below is a
plain `Material` (not instanced from a complex master unless noted) using only a
`Constant3Vector` (or `VertexColor`) into Base Color, Roughness pinned high (~0.8-0.9, no
specular highlights to keep the toy-like read), Metallic = 0.

| Material            | Base Color (approx)      | Used on                              |
|----------------------|---------------------------|----------------------------------------|
| M_House_Wall_01..06  | 6 bright, distinct hues   | House walls, one hue per house so silhouettes read from across the map |
| M_Road_Asphalt       | dark neutral grey         | Road cylinder                        |
| M_Sidewalk           | light grey                | Sidewalks                            |
| M_Grass              | saturated green           | Yards                                |
| M_Roof_01..03        | 3 contrasting hues        | Roof shapes, paired per-house with wall hue |
| M_Prop_Wood          | warm brown                | Furniture                            |
| M_Prop_Metal         | cool grey/blue            | Electronics/safes                    |
| M_Objective_Highlight | emissive gold, driven by a scalar param `HighlightStrength` | Thin outline/fresnel pulse on `ASHObjectivePoint` meshes so they read as interactable at a glance |
| M_Character_Skin_01..04 | 4 distinct skin tones, exaggerated saturation | Resident/Player skin |
| M_Character_Outfit_Calm/Alert/Sensitive/Patrol | 4 distinct hues matching archetype | Resident outfits - lets players learn archetype by color (Docs/BLUEPRINT_CONSTRUCTION.md) |

## Master material (optional, for scale)
If hand-authoring 15+ single-color materials is too slow, build one
`M_Master_Stylized` with a `VectorParameter BaseColorTint` and `ScalarParameter Roughness`,
then create a `Material Instance Constant` per entry in the table above, only setting the
tint. Functionally identical result, faster iteration.

## Creating these via Python (optional)
`unreal.AssetToolsHelpers.get_asset_tools().create_asset(name, "/Game/Materials",
unreal.Material, unreal.MaterialFactoryNew())` creates an empty Material asset; wiring the
Constant3Vector -> Base Color node graph via `unreal.MaterialEditingLibrary` is possible but
verbose per-material, so it's left as a manual step here (15 materials, ~1 minute each in the
Material Editor) rather than a fragile generated script.
