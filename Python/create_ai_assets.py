"""
Section 6/8/29: creates BB_Resident (BlackboardData) with the keys listed in
Docs/BEHAVIOR_TREE_SPEC.md, and a BT_Resident (BehaviorTree) shell that references it.

LIMITATION: building the actual Behavior Tree node graph (the Selector/Sequence/Task/
Decorator/Service wiring) programmatically is not reliably exposed through the stable
Unreal Python API across 5.x versions, so this script only creates the two assets and
assigns BT_Resident.BlackboardAsset = BB_Resident. Follow Docs/BEHAVIOR_TREE_SPEC.md to
build the node graph by hand in the Behavior Tree editor (a five-minute job once the C++
task/service/decorator classes below are compiled and visible in the node palette).
"""
import unreal

BLACKBOARD_PACKAGE = "/Game/AI/Blackboards"
BLACKBOARD_NAME = "BB_Resident"
BEHAVIOR_TREE_PACKAGE = "/Game/AI/BehaviorTrees"
BEHAVIOR_TREE_NAME = "BT_Resident"

# (KeyName, KeyType short name) - resolved via unreal.load_class below. The BlackboardKeyType_*
# classes are not marked BlueprintType, so on UE 5.8 they are NOT exposed as `unreal.BlackboardKeyType_*`
# module attributes (that raises AttributeError) - they must be loaded by native class path instead.
KEYS = [
	("TargetActor", "Object"),
	("LastHeardLocation", "Vector"),
	("SuspicionLevel", "Float"),
	("IsInvestigating", "Bool"),
	("HasDetectedPlayer", "Bool"),
	("HomeLocation", "Vector"),
	# DetectionState (Enum, EResidentState) must be added manually in the Blackboard editor -
	# UBlackboardKeyType_Enum requires an editor-side enum asset reference that Python's
	# stable API does not expose a clean setter for; see Docs/BEHAVIOR_TREE_SPEC.md.
]


def _bb_key_type_class(short_name):
	class_path = f"/Script/AIModule.BlackboardKeyType_{short_name}"
	key_type_class = unreal.load_class(None, class_path)
	if key_type_class is None:
		raise RuntimeError(f"Could not load class {class_path} - check engine module path for this UE version")
	return key_type_class


def _create_asset(name, package_path, asset_class, factory):
	asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
	full_path = f"{package_path}/{name}"
	if unreal.EditorAssetLibrary.does_asset_exist(full_path):
		unreal.log(f"Asset already exists, skipping: {full_path}")
		return unreal.load_asset(full_path)
	asset = asset_tools.create_asset(name, package_path, asset_class, factory)
	unreal.log(f"Created asset: {full_path}")
	return asset


def run():
	blackboard = _create_asset(
		BLACKBOARD_NAME, BLACKBOARD_PACKAGE, unreal.BlackboardData, unreal.BlackboardDataFactory()
	)

	if blackboard:
		# str() the entry_name explicitly - it comes back as an FName, and comparing it
		# directly against the plain `str` values in KEYS silently never matched (found the
		# hard way: two successful runs each re-appended all 6 keys as duplicates instead of
		# skipping them).
		existing_names = {str(entry.get_editor_property("entry_name")) for entry in blackboard.get_editor_property("keys")}
		keys = list(blackboard.get_editor_property("keys"))
		for key_name, key_type_short_name in KEYS:
			if key_name in existing_names:
				continue
			entry = unreal.BlackboardEntry()
			entry.set_editor_property("entry_name", key_name)
			key_type_class = _bb_key_type_class(key_type_short_name)
			# `outer` must be a real UObject - `entry` is a struct (FBlackboardEntry), not one,
			# so it can't be used here (that raised "Cannot nativize 'BlackboardEntry' as 'Object'").
			# The blackboard asset itself is a valid, persistent UObject outer.
			entry.set_editor_property("key_type", unreal.new_object(key_type_class, outer=blackboard))
			keys.append(entry)
		blackboard.set_editor_property("keys", keys)
		unreal.EditorAssetLibrary.save_loaded_asset(blackboard)

	behavior_tree = _create_asset(
		BEHAVIOR_TREE_NAME, BEHAVIOR_TREE_PACKAGE, unreal.BehaviorTree, unreal.BehaviorTreeFactory()
	)
	if behavior_tree and blackboard:
		behavior_tree.set_editor_property("blackboard_asset", blackboard)
		unreal.EditorAssetLibrary.save_loaded_asset(behavior_tree)

	unreal.log("BB_Resident / BT_Resident ready. Build the node graph per Docs/BEHAVIOR_TREE_SPEC.md.")


if __name__ == "__main__":
	run()
