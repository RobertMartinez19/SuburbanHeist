"""
Section 25/29: creates the Content/ folder structure via the Unreal Editor Python API.
Run inside the Editor: Window > Developer Tools > Output Log, then
`exec(open(r"<project>/Python/create_folder_structure.py").read())`, or add this
project's Python/ dir to Project Settings > Python and run via `import create_folder_structure`.
"""
import unreal

FOLDERS = [
	"/Game/Blueprints/Characters",
	"/Game/Blueprints/AI",
	"/Game/Blueprints/Objectives",
	"/Game/Blueprints/Environment",
	"/Game/Blueprints/Gameplay",
	"/Game/Blueprints/Interaction",
	"/Game/Blueprints/Audio",
	"/Game/Blueprints/UI",
	"/Game/AI/BehaviorTrees",
	"/Game/AI/Blackboards",
	"/Game/AI/EQS",
	"/Game/Maps",
	"/Game/Materials",
	"/Game/Meshes",
	"/Game/Audio",
	"/Game/UI",
	"/Game/Data",
]


def run():
	editor_asset_lib = unreal.EditorAssetLibrary
	for folder in FOLDERS:
		if not editor_asset_lib.does_directory_exist(folder):
			editor_asset_lib.make_directory(folder)
			unreal.log(f"Created folder: {folder}")
		else:
			unreal.log(f"Folder already exists: {folder}")


if __name__ == "__main__":
	run()
