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

		# NOTE: unreal.AssetImportTask's generic import pipeline fails to resolve
		# DataTableFactory for the '.csv' extension on this engine build ("Unknown
		# extension 'csv'" from LogAssetTools, silently producing no asset even though
		# import_asset_tasks() doesn't raise). Create the DataTable asset directly and
		# fill it via DataTableFunctionLibrary instead, which is the reliable path.
		factory = unreal.DataTableFactory()
		factory.struct = row_struct

		full_path = f"{entry['package_path']}/{entry['asset_name']}"
		if unreal.EditorAssetLibrary.does_asset_exist(full_path):
			unreal.log(f"Asset already exists, skipping: {full_path}")
			continue

		data_table = asset_tools.create_asset(entry["asset_name"], entry["package_path"], unreal.DataTable, factory)
		if not data_table:
			unreal.log_error(f"Failed to create DataTable asset at {full_path}")
			continue

		with open(csv_path, "r", encoding="utf-8-sig") as f:
			csv_text = f.read()

		# fill_data_table_from_csv_string returns a bool - True on success - not a list of
		# problem strings, despite what the variable name below used to imply.
		success = unreal.DataTableFunctionLibrary.fill_data_table_from_csv_string(data_table, csv_text)
		if not success:
			unreal.log_error(f"Failed to populate {full_path} from {csv_path}")

		unreal.EditorAssetLibrary.save_loaded_asset(data_table)
		unreal.log(f"Imported DataTable {full_path}")


if __name__ == "__main__":
	run()
