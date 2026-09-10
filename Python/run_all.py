"""
Runs every editor automation script in the correct order (Section 29).
Usage (Python console in-editor, from the project root):
	import sys; sys.path.append(r"<project>/Python")
	import run_all; run_all.run()
"""
import unreal


def run():
	import create_folder_structure
	import create_data_tables
	import create_ai_assets

	unreal.log("=== SuburbanHeist automation: folders ===")
	create_folder_structure.run()

	unreal.log("=== SuburbanHeist automation: data tables ===")
	create_data_tables.run()

	unreal.log("=== SuburbanHeist automation: AI assets (BB_Resident / BT_Resident) ===")
	create_ai_assets.run()

	unreal.log(
		"=== Done. Open an empty level and run generate_culdesac_blockout.py separately "
		"to greybox the map (it needs a level open, unlike the asset-creation scripts "
		"above), then follow Docs/BEHAVIOR_TREE_SPEC.md to finish BT_Resident's node graph. ==="
	)


if __name__ == "__main__":
	run()
