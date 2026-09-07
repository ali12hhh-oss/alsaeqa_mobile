# ALSAEQA — Sequential Skills & Weapon Adventure Progression

## Core rule
The hero does **not** begin the game with the full moveset or arsenal. Progression is discovered through story, exploration, trials, bosses, secrets, and dedicated adventures.

A new power is never granted simply because the player reached a menu or bought it. The player must reach the correct story stage and complete the associated discovery event. Weapons are physical discoveries and are acquired one at a time.

## Stage sequence

| Stage | Main discovery | Power | Weapon / adventure |
|---|---|---|---|
| 0 | Mine escape begins | None | Improvised tools only |
| 1 | Ancient thunder mark | ThunderSpark | **RustedBlade** — found after escaping the mine and defeating the first armed overseer |
| 2 | Storm-blood awakening | LightningDash | **HunterDagger** — recovered during a forest tracking/ambush mission |
| 3 | Mountain trial | ThunderJump | **IronSpear** — earned after the first traversal trial and spear guardian encounter |
| 4 | Thunder shrine | StormFist | Weapon mastery quest; no automatic weapon grant |
| 5 | Guardian trial | LightningShield | **ThunderHammer** — recovered from an ancient sealed chamber after solving a power puzzle |
| 6 | Eye of the storm | ThunderEye | Secret-area exploration becomes available |
| 7 | Echoes of the ancestors | ThunderEcho | **StormSpear** — acquired through a multi-part relic hunt and guardian fight |
| 8 | Call of the storm | StormSummon | Major storm trial; this is a power milestone, not a free early skill |
| 9 | Heir of thunder | — | **ThunderBlade** — late-game legendary weapon earned through a dedicated family/giant adventure |

The exact stage names and level layouts can evolve, but the progression principle must remain: **discover → prove → acquire → master → unlock new routes**.

## Weapon adventures

Every major weapon has a small adventure around it rather than appearing as random loot:

1. **Rusted Blade** — first real weapon; establishes basic combat.
2. **Hunter Dagger** — fast attacks and stealth-oriented encounters.
3. **Iron Spear** — reach, spacing, and traversal interactions.
4. **Thunder Hammer** — heavy impact, armor breaking, environmental destruction.
5. **Storm Spear** — thunder-enhanced reach and selected ancient mechanisms.
6. **Thunder Blade** — legendary endgame weapon tied to the hero's lineage.

A weapon quest can contain: clue → exploration → environmental obstacle → combat trial → mini-boss/boss → discovery scene → pickup → short mastery sequence.

## Ability discovery rules

- No supernatural ability is active at stage 0.
- The player can see that unknown powers exist, but locked powers must not be usable.
- Discovery events should be explicit and memorable: ancient symbols, family relics, storms, visions, trials, or boss encounters.
- A newly discovered power can also unlock traversal routes and secrets, making progression part of exploration rather than a simple skill menu.
- Abilities should have cooldown/resource rules after discovery so unlocking a power does not make the rest of the combat obsolete.

## Failure protection

Discovery must be granted only once. Re-entering an area, retrying a boss, or replaying a cinematic must not duplicate a weapon or power.

The progression component therefore stores:
- current story stage;
- discovered powers;
- acquired weapons;
- unique discovery identifiers supplied by level/quest logic.

## Important implementation boundary

C++ progression contracts are stored in `Source/ALSAEQA/Progression/`. Actual weapon meshes, animation montages, VFX, audio, cinematics, map layouts, and pickup actors must be authored/assigned in Unreal Editor as project content. This repository currently does not claim those binary assets exist.
