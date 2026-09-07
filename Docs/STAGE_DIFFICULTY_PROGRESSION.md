# ALSAEQA — Strict Stage Difficulty Progression

## Core rule

Every stage must be harder than the stage immediately before it. Difficulty is continuous from stage 1 through stage 70; it is never reset when entering a new region.

## Runtime baseline

The stage-flow system automatically derives a monotonic baseline from the stage number. Designers may raise these values for a particular stage, but the system will not allow a configured value to fall below its stage baseline.

- Difficulty rating: `1.0 + 0.10 × (stage - 1)` capped at `10.0`.
- Enemy health baseline: `1.0 + 0.015 × (stage - 1)`.
- Enemy damage baseline: `1.0 + 0.012 × (stage - 1)`.
- Enemy aggression baseline: `1.0 + 0.010 × (stage - 1)`.
- Hazard baseline: `1.0 + 0.014 × (stage - 1)`.

The current enemy implementation also increases pursuit speed, detection pressure, attack frequency and attack damage continuously with stage progression.

## How difficulty increases

Difficulty must not rely on health inflation alone. Each later stage can add one or more of:

1. New enemy types and combinations.
2. Better enemy positioning and coordinated pressure.
3. Faster or more varied enemy attacks.
4. More demanding boss phases.
5. More dangerous terrain, storms, falls and traversal sequences.
6. More complex puzzles and multi-step objectives.
7. Tighter resource and stamina decisions.
8. Stronger requirements for combining powers, weapons and mounts.
9. More consequential optional routes and ambushes.
10. Longer encounters with intelligent checkpoints so failure is challenging but not frustrating.

## Fairness rule

A later stage must be harder, but it must remain beatable using abilities and knowledge that the player has reasonably earned by that point. Difficulty increases should teach a mechanic before demanding mastery of it.

## Death interaction

The player has one soul/life, not a stock of multiple lives. Death does not restart the entire stage. The player returns to the latest saved checkpoint after a short death sequence. Permanent discoveries, powers, weapons, companion progress, family evidence and tamed mounts remain saved.

The companion does not automatically die when the player dies. She remains an independent combatant and the player respawns without deleting her story or progress.

## Milestones

- 1–10: learning, basic combat and survival.
- 11–20: combined powers, weapons and stronger encounters.
- 21–24: preparation and higher exploration pressure.
- 25–40: increasingly dangerous family-search routes and investigations.
- 41–44: the companion actively supports the hero's family search.
- 45–55: traversal, taming, riding and increasingly dangerous mounted routes.
- 56–59: ThunderBeast trust and storm territory.
- 60–69: specialized ThunderBeast abilities, high-risk traversal and advanced combat.
- 70: highest difficulty and the full-system culmination.
