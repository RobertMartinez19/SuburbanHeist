"""
Section 26/29: imports Content/Data/*.csv into real DataTable assets bound to the C++ row
structs (FSHObjectiveDataRow, FSHResidentArchetypeDataRow). Requires the C++ module to have
been compiled first (the row struct classes must exist for the DataTableFactory to bind to).

Run inside the Editor after create_folder_structure.py.
"""
import unreal
import os

PROJECT_CONTENT_DIR = unreal.Paths.project_content_dir()

DATATABLES = [
	{
		"csv": "Data/DT_Objectives.csv",
		"asset_name": "DT_Objectives",
		"package_path": "/Game/Data",
		"row_struct": "/Script/SuburbanHeist.SHObjectiveDataRow",
	},
	{
		"csv": "Data/DT_ResidentArchetypes.csv",
		"asset_name": "DT_ResidentArchetypes",
		"package_path": "/Game/Data",
		"row_struct": "/Script/SuburbanHeist.SHResidentArchetypeDataRow",
	},
]


def run():
	asset_tools = unreal.AssetToolsHelpers.get_asset_tools()

	for entry in DATATABLES:
		csv_path = os.path.join(PROJECT_CONTENT_DIR, entry["csv"])
		if not os.path.exists(csv_path):
			unreal.log_error(f"CSV not found: {csv_path}")
			continue

		row_struct = unreal.load_object(None, entry["row_struct"])
		if not row_struct:
			unreal.log_error(
				f"Row struct {entry['row_struct']} not found - compile the SuburbanHeist "
				"C++ module first (SHTypes.h defines FSHObjectiveDataRow / "
				"FSHResidentArchetypeDataRow)."
			)
			continue

		factory = unreal.DataTableFactory()
		factory.struct = row_struct

		task = unreal.AssetImportTask()
		task.filename = csv_path
		task.destination_path = entry["package_path"]
		task.destination_name = entry["asset_name"]
		task.replace_existing = True
		task.automated = True
		task.save = True
		task.factory = factory

		unreal.AssetToolsHelpers.get_asset_tools().import_asset_tasks([task])
		unreal.log(f"Imported DataTable {entry['package_path']}/{entry['asset_name']}")


if __name__ == "__main__":
	run()
