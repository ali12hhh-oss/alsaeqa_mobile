# ALSAEQA Visual Implementation

The repository now has an offline-safe runtime visual layer that does not require external marketplace assets.

## Runtime characters
- Hero: readable humanoid silhouette, head/body/shoulders, weapon prop and storm light.
- Companion: distinct smaller humanoid silhouette and weapon prop.
- Enemy: heavier silhouette and melee weapon prop.
- Boss: enlarged silhouette with storm illumination.
- Giant Snake: elongated body/head silhouette.
- Mount: body/head silhouette.

## Automatic attachment
`UALSAEQARuntimeVisualSubsystem` scans spawned `ACharacter` instances and attaches `UALSAEQARuntimeVisualComponent` automatically. Class names containing Boss, Snake, Enemy, Companion or Mount select the matching visual role; other characters use the Hero role.

## Animation
The runtime visual component performs a lightweight procedural rotation animation on generated mesh parts. It is intentionally asset-free and suitable as a fallback while production skeletal meshes and animation montages are authored.

## World
The existing runtime world builder remains the playable greybox foundation: ground tiles, forest, rocks, ancient ruins, broken bridge, stepping stones, climbing route and thunder altar.

## Production asset path
For final art, replace the generated primitive parts with imported skeletal meshes, then assign Animation Blueprints/montages to the same Player, Creatures, Boss, Companions and Combat systems. No gameplay API needs to change.

## Important limitation
GitHub text APIs cannot author binary FBX/GLB skeletal meshes, animation clips, textures or Unreal `.uasset` files. Therefore this pass implements a real in-engine visual fallback and integration layer rather than pretending binary art assets were created or imported.
