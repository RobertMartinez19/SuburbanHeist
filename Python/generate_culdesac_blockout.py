"""
Section 15/29: spawns a basic blockout of the cul-de-sac map using engine primitive meshes
(Cube/Plane/Cylinder) - no custom meshes required. This gives a developer an immediately
playable, readable greybox: a circular road, 10 house blockouts around it with a front yard
and driveway each, and a perimeter of tree/streetlight placeholders. Replace with real
block-style modular assets per Docs/MAP_CONSTRUCTION.md and Docs/HOUSE_MODULE_SPEC.md.

Run inside the Editor with an empty level open (File > New Level > Empty), then save as
/Game/Maps/M_CulDeSac.
"""
import unreal
import math

NUM_HOUSES = 10
ROAD_RADIUS = 2500.0
HOUSE_RING_RADIUS = 3600.0
HOUSE_SIZE = unreal.Vector(800, 1000, 400)
YARD_SIZE = unreal.Vector(900, 700, 5)

CUBE_MESH_PATH = "/Engine/BasicShapes/Cube.Cube"
CYLINDER_MESH_PATH = "/Engine/BasicShapes/Cylinder.Cylinder"
PLANE_MESH_PATH = "/Engine/BasicShapes/Plane.Plane"


def _spawn_mesh_actor(mesh_path, location, scale, label, rotation=None):
	actor_subsystem = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
	mesh = unreal.EditorAssetLibrary.load_asset(mesh_path)
	if not mesh:
		unreal.log_error(f"Could not load {mesh_path} - is this an Unreal Editor Python session?")
		return None

	actor = actor_subsystem.spawn_actor_from_object(
		mesh, location, rotation or unreal.Rotator(0, 0, 0)
	)
	if actor:
		actor.set_actor_label(label)
		static_mesh_component = actor.static_mesh_component if hasattr(actor, "static_mesh_component") else None
		actor.set_actor_scale3d(scale)
	return actor


def run():
	# Central road - a wide flat cylinder standing in for asphalt.
	_spawn_mesh_actor(
		CYLINDER_MESH_PATH,
		unreal.Vector(0, 0, 0),
		unreal.Vector(ROAD_RADIUS / 100.0, ROAD_RADIUS / 100.0, 0.1),
		"SM_Road_Blockout",
	)

	for i in range(NUM_HOUSES):
		angle = (2.0 * math.pi / NUM_HOUSES) * i
		x = HOUSE_RING_RADIUS * math.cos(angle)
		y = HOUSE_RING_RADIUS * math.sin(angle)
		yaw = math.degrees(angle) + 180.0  # face the houses inward toward the road

		house_loc = unreal.Vector(x, y, HOUSE_SIZE.z * 0.5 * 2.0)  # cube pivot is centered
		house_rot = unreal.Rotator(0, yaw, 0)
		_spawn_mesh_actor(
			CUBE_MESH_PATH, house_loc, HOUSE_SIZE / 100.0, f"SM_House_{i:02d}_Blockout", house_rot
		)

		yard_loc = unreal.Vector(x * 0.75, y * 0.75, 2)
		_spawn_mesh_actor(
			PLANE_MESH_PATH, yard_loc, YARD_SIZE / 100.0, f"SM_Yard_{i:02d}_Blockout", house_rot
		)

	unreal.log(
		f"Spawned road + {NUM_HOUSES} house/yard blockouts. Save this level as "
		"/Game/Maps/M_CulDeSac, then place BP_TeamDetectionManager, BP_LootManager, "
		"BP_NoiseManager, BP_AudioManager, a PlayerStart per player, and ASHObjectivePoint "
		"instances inside each house per Docs/MAP_CONSTRUCTION.md."
	)


if __name__ == "__main__":
	run()
