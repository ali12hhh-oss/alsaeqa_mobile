# ALSAEQA Real Assets

The canonical visual source is kept in the original ALSAEQA repository Releases and is downloaded by CI. The mobile project never replaces these assets with procedural placeholders.

## Source packs

- `ALSAEQA_REAL_ASSETS.zip` — canonical real asset pack
- `ALSAEQA_EXTRA_MONSTERS.zip.zip` — extra creatures, including flying creatures
- `ALSAEQA_EXTRA_ULTIMATE_MODULAR_RUINS.zip.zip` — modular ruins/dungeons/prisons/caves/walls

## Editable policy

The original source files are preserved during CI extraction under `build/source_assets/`. Converted mobile files are generated under `mobile/assets/converted/` and are linked by `mobile/assets/asset_manifest.json`.

Nothing in this folder is intended to become a locked, flattened replacement for the source. Artists can replace or re-convert a source asset without changing gameplay code by keeping the same logical asset ID in the manifest.
