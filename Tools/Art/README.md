# ALSAEQA Real Character Customization

The final character workflow starts from a real rigged mesh and preserves its armature/weights. It never creates replacement bodies from primitive shapes.

1. Acquire an approved pack from its official source.
2. Import the real mesh into Blender.
3. Run `Tools/Art/customize_real_character.py` with the requested role/variant.
4. Manually/art-direct outfit, hair, accessories, weapon silhouette and material details as needed.
5. Export FBX/GLB with the armature and animations preserved.
6. Import into Unreal 5.8 with `Tools/Unreal/ImportRealArt.py`.
7. Assign the resulting mesh/Anim Blueprint to the gameplay actor or visual profile.
8. Verify LODs, materials, retargeting, collision and Android scalability.

The script is deliberately conservative: it refuses to fabricate a body from cubes/spheres and only operates on existing mesh objects from the imported asset.
