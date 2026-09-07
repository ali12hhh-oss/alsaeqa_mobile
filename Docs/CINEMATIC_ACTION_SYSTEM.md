# ALSAEQA — Cinematic Action System

The game uses short, gameplay-driven cinematic moments rather than removing player control for long sequences.

## Combat moments
- Perfect dodge: brief time dilation, close camera, impact emphasis, then immediate return to control.
- Parry/counter: slow the decisive contact and transition into the counter animation.
- Finisher: contextual camera, character facial reaction, weapon impact, particles and controlled slow motion.
- Thunder ability impact: camera response, lightning flash, environmental reaction and short time dilation.
- Near-death escape: brief tunnel/impact treatment without disabling recovery input.
- Boss reveal: controlled camera framing and environmental reaction.

## Traversal moments
- Narrow ledge slips and recoveries.
- Rock or bridge collapse with controlled fall camera.
- Last-second grab to a ledge.
- Climbing failure followed by recovery.
- Rope/chain swing and landing.
- Long jump landing with dust and camera shake.
- Cave-in escape.

## Rescue moments
- Companion catches the hero during selected scripted falls.
- Hero rescues companion using a power, rope, weapon or environmental object.
- Both characters can become temporarily vulnerable during a rescue, creating a gameplay decision rather than a passive cutscene.
- Rescue sequences must respect player context and never trigger repeatedly for the same event.

## Character performance
- Facial expression states: fear, pain, anger, determination, surprise, relief and exhaustion.
- Reactions are driven by gameplay events and animation/MetaHuman or equivalent facial assets when available.
- Body language changes with stamina, injuries, weather and story state.

## Giant snake encounters
Giant snakes are both enemies and traversal hazards. They can:
- Hide in vegetation, ruins, caves or water.
- Ambush from above, below or around terrain.
- Coil around pillars, bridges and narrow routes.
- Block a path without immediately attacking.
- Chase the player through a traversal section.
- Force a timing-based dodge or climb sequence.
- Enter a short cinematic action moment for a near-miss, bite dodge or environmental takedown.
- Act as mini-bosses with readable attack phases and weaknesses.

## Slow motion rules
- Use short, purposeful bursts rather than constant slow motion.
- Default action moments target roughly 0.15–0.35 world time scale.
- Camera effects, animation, sound and VFX should be authored separately so mobile quality tiers can reduce cost.
- Never rely on slow motion alone for gameplay readability.

## Safety and performance
- Cinematic moments must be interruptible where appropriate.
- Avoid expensive post-processing on the baseline mobile profile.
- Camera shakes and effects require intensity limits.
- Every cinematic trigger needs a cooldown or one-shot state to prevent accidental loops.
- Binary Unreal assets, animations, facial rigs and final cinematic sequences must be authored in Unreal Editor/content tools; this document defines their gameplay contract.
