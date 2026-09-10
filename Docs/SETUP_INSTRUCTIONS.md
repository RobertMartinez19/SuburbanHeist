# Setup Instructions

## Requirements
- Unreal Engine 5.4.x (match `EngineAssociation` in `SuburbanHeist.uproject`)
- Visual Studio 2022 (Windows) or Xcode (Mac) with UE5 C++ workflow installed
- Python Editor Script Plugin (bundled with UE5, enabled in the `.uproject`)

## First-time setup
1. Right-click `SuburbanHeist.uproject` -> **Generate Visual Studio project files** (or run
   `UnrealBuildTool` manually) to produce the `.sln`.
2. Open the generated solution, build `SuburbanHeistEditor` (Development Editor config).
   This compiles every C++ class in this repo - GameMode, all managers, the Resident AI, the
   BT nodes, etc. Fix any engine-version API drift per `Docs/LIMITATIONS.md` if needed.
3. Open `SuburbanHeist.uproject` in the Editor.
4. Create a new empty level and save it as `/Game/Maps/M_CulDeSac` (this matches
   `Config/DefaultEngine.ini`'s `GameDefaultMap`/`EditorStartupMap`).
5. In the Python console (Window > Developer Tools > Output Log has a Python tab, or Window >
   Cmd Console set to Python), run:
   ```python
   import sys
   sys.path.append(r"<path-to-project>/Python")
   import run_all
   run_all.run()
   ```
   This creates the `Content/` folder structure, imports `DT_Objectives`/
   `DT_ResidentArchetypes` from the CSV seeds, and creates the `BB_Resident`/`BT_Resident`
   asset shells (see `Docs/LIMITATIONS.md` for what it can't do - the BT node graph).
6. With the empty `M_CulDeSac` level still open, run
   `Python/generate_culdesac_blockout.py` the same way to greybox the map.
7. Follow, in order:
   - `Docs/BLUEPRINT_CONSTRUCTION.md` (Input assets, all `BP_*` classes)
   - `Docs/BEHAVIOR_TREE_SPEC.md` (finish `BT_Resident`'s node graph)
   - `Docs/MAP_CONSTRUCTION.md` / `Docs/HOUSE_MODULE_SPEC.md` (flesh out the greybox houses)
   - `Docs/MATERIALS_SPEC.md` (stylized materials)
   - `Docs/UI_SPEC.md` (`WBP_HUD` and end-of-match overlays)
8. Set the map's **World Settings > GameMode Override** to `BP_GameMode` (or leave the
   `Config/DefaultEngine.ini` default, which already points at the C++ `ASHGameMode`).
9. Play In Editor with **Number of Players = 2-4**, **Net Mode = Play As Listen Server**
   (Editor Preferences > Level Editor > Play) to test multiplayer locally before testing
   over LAN/online.

## Running a LAN/dedicated test
- Package a development build (`File > Package Project`), run one instance with
  `-server -log`, connect others via `open <server-ip>`.
- `Config/DefaultEngine.ini` already sets `OnlineSubsystem` to `Null` for LAN-friendly
  defaults; swap in Steam/EOS config there if you need online matchmaking beyond direct-IP.

## Debug mode
`USHGameInstance::bDebugModeEnabled` defaults to on outside Shipping builds - see
`Docs/DEBUG_COMMANDS.md` for the full console command list.
