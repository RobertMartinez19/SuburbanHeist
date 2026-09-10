# Binary Asset Limitation

This environment can write text/source files to disk but **cannot produce Unreal's binary
`.uasset` / `.umap` formats**. Those are serialized by the Editor itself (versioned package
format, cooked references, GUIDs) and there is no supported way to hand-author them outside
UnrealEd.

Every asset that would normally be a `.uasset` is instead delivered as one of:

1. **A C++ class** (compiles to a `UCLASS`/`USTRUCT`/`UENUM` that the Editor can subclass into
   a Blueprint) - this covers every gameplay system in this project.
2. **A CSV seed file** under `Content/Data/`, importable directly into a real `UDataTable`
   asset via the DataTable factory (or `Python/create_data_tables.py`).
3. **An exact construction spec** under `Docs/` - class hierarchy, component list, property
   values, node-by-node graph logic - precise enough that following it mechanically in the
   Editor reproduces the intended asset. Nothing here is a placeholder pretending to be a
   `.uasset`.
4. **A Python Unreal Editor automation script** under `Python/` that creates the asset
   programmatically when run inside the Editor (DataTables, Blackboard/BehaviorTree shells,
   blockout geometry). Where the stable Python API doesn't expose something reliably (the
   Behavior Tree's internal node graph, Enhanced Input asset factories on some 5.x point
   releases, the exact `UAudioCaptureComponent` envelope-follower delegate signature), the
   script or C++ comment says so explicitly and points at the manual doc instead of silently
   producing something wrong.

Nothing under `Content/` in this repo is a real Unreal asset unless it's a `.csv` explicitly
documented as a DataTable seed. Everything else needed to see the game running is: compile
the C++ module, then follow `Docs/SETUP_INSTRUCTIONS.md`.

## Specific version-sensitive spots to double check against your installed 5.4.x

- `USHMicrophoneNoiseComponent` binds `UAudioCaptureComponent::OnAudioEnvelopeValue`, assuming
  a single-`float` delegate signature (the common SynthComponent envelope-follower pattern).
  If your engine build's signature differs, adjust `HandleAudioEnvelopeValue`'s signature to
  match `SynthComponent.h` / `AudioCaptureComponent.h`.
- Enhanced Input `UInputAction` / `UInputMappingContext` asset creation via Python factories
  is not exercised by the scripts here - create `IMC_Default` and its `IA_*` actions manually
  per `Docs/BLUEPRINT_CONSTRUCTION.md` (a two-minute job per asset in the Content Browser).
