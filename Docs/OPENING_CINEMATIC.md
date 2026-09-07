# ALSAEQA — Professional Opening Cinematic

## Status
The project already contained the opening story seed in Stage 1 and a general cinematic-action contract. This document is the production blueprint for the opening film and must follow `HERO_ORIGIN_AND_MAIN_STORY.md` as the canonical story source.

## Goal
The first minutes must immediately establish that ALSAEQA is a cinematic third-person adventure, not a prototype. The opening should feel mysterious, emotional and dangerous, then hand control to the player without an abrupt transition.

Target runtime: **30–45 seconds** for the mandatory first-launch prologue, with an optional extended 3–4 minute presentation assembled from the same authored shots. The 30–45 second cut is the canonical mobile-safe opening and must still preserve the complete emotional chain: family memory → attack → hiding → capture/escape → slavery → mine collapse → relic → first awakening → escape → Stage 1 hand-off.

## Canonical story constraint
Thunder Giants is the name of the hero's ruling family/bloodline, **not a literal race of giant-sized humans**. Any large silhouette, statue, armor, ruin or perspective shot must not imply that the family members were physically giant-sized. The cinematic must preserve the distinction between the original force that destroyed the Thunder Kingdom and the different criminals/slavers who later enslaved the child.

## Narrative structure

### Sequence 01 — The Storm Before Memory (0:00–0:06)
- Begin in near darkness with distant wind, low thunder and stone vibration.
- Reveal a vast ancient landscape through clouds, rain and intermittent lightning.
- Show monumental Thunder-era ruins, family architecture, heraldry and human-scale ancestral figures through carefully controlled perspective.
- Do not explain the entire mythology. The audience should understand that an ancient ruling bloodline once protected the realm.
- Lightning briefly reveals enormous ruins and a monumental path disappearing into the storm.

### Sequence 02 — The Fall of the Thunder Legacy (0:06–0:13)
- Move through the ancient ruins as the storm intensifies.
- Show the Thunder ruling family being overwhelmed by the enemy force through silhouettes, distant movement, firelight, collapsing architecture and reactions rather than graphic violence.
- A family symbol is protected during the chaos.
- The young child is present within the family home during the attack.
- The sequence establishes the father's and brother's deaths and the capture of the mother and surviving siblings without graphic depiction.
- The mother hides the child behind a chest/large storage box; the child witnesses the destruction and later escapes the immediate attackers.

### Sequence 03 — The Second Captors and the Heir (0:13–0:20)
- Transition from the childhood escape to the later period of enslavement.
- Make it clear through visual language that the people who enslave the child are a **different group** from the attackers who destroyed the Thunder Kingdom.
- Reveal the hero years later in confinement inside the mine.
- Close shots establish breathing, restrained hands, dirt, exhaustion, scars and determination.
- The hero does not understand his Thunder lineage and does not remember his true name.
- A faint ancient mark or symbol becomes visible in the mine environment.

### Sequence 04 — The Collapse (0:20–0:27)
- A structural failure begins far away.
- Dust, rocks, chains, wood and debris react physically.
- The camera alternates between controlled cinematic angles and close survival shots.
- The hero is caught in the collapse and falls into a deep underground chamber.
- Include a signature survival beat as he struggles against the fall and reaches safety.

### Sequence 05 — The First Awakening (0:27–0:37)
- In the deep chamber, the hero discovers an ancient object, mechanism, relic, symbol or puzzle-like source connected to the Thunder lineage.
- His distinctive body mark reacts when he approaches/touches it.
- The dormant inherited power awakens for the first time.
- The first manifestation is restrained and survival/traversal-oriented: a rapid climb, launch, grapple-like movement or similar ability that allows him to escape the chamber.
- The hero escapes the mine and reaches a position where he can see the outside world.
- The effect remains small and mysterious; do not turn him into an all-powerful warrior.

### Sequence 06 — Hand-off to Stage 1 (0:37–0:45)
- The hero has escaped immediate captivity but realizes the workers he left behind are still trapped.
- A final wide shot establishes the mine, surrounding route and the wider world.
- The game title **ALSAEQA / الصاعقة** appears with restrained thunder illumination, not a cheap flash.
- Music resolves into the game's main motif.
- The camera transitions into the exact gameplay camera position for Stage 1.
- HUD remains hidden until player control is confirmed.
- Player control begins at the post-awakening location defined by Stage 1.
- The first playable objective is **Return to the mines, rescue the workers, and confront the slavers controlling the mine**.

## Camera language
- Use deliberate cinematic composition: wide environmental reveals, low-angle power shots, intimate facial/body shots, controlled tracking and motivated camera movement.
- Avoid constant camera shake. Reserve it for structural collapse, thunder impact and major danger.
- Use depth, foreground objects, silhouettes and environmental occlusion to create scale.
- Never use a random camera move just to look cinematic; every shot must reveal information, emotion or danger.

## Character performance
- Child hero: fear -> hiding -> shock -> escape instinct.
- Adult hero: exhaustion -> confusion -> pain -> determination -> wonder at the first awakening.
- Ancient family members: communicate authority through posture, costume, environment and human-scale composition rather than literal giant anatomy.
- Facial animation, clothing, hair and body motion must use the project's real character/animation assets when available.

## Audio direction
- Start with environmental sound before music.
- Layer low thunder, stone vibration, chains, breathing, distant impacts, debris and wind.
- Music enters gradually instead of starting at full volume.
- Use silence immediately before the first meaningful thunder response.
- All final audio must be licensed/owned for commercial use before shipping.

## VFX direction
- Real Niagara-based lightning/thunder effects are required for the hero's awakening and major storm beats.
- Dust, debris, sparks and atmospheric particles must respond to the event rather than appear as disconnected effects.
- The first spark is intentionally small; later powers can become spectacular as progression advances.
- Do not use primitive placeholder meshes or placeholder VFX in the final cinematic.

## Gameplay hand-off rules
- The opening is one-shot per new game.
- It must never replay unexpectedly after a checkpoint reload.
- Skip is optional and should be disabled during critical loading/initialization boundaries; if enabled, it must transition cleanly to Stage 1.
- Completion must set the opening-complete state before normal Stage 1 gameplay starts.
- If a final cinematic asset is unavailable, the build must fail cinematic validation rather than silently substituting primitive placeholder content.

## Unreal authoring contract
The C++ cinematic director exposes an opening lifecycle and a Blueprint event. The actual Level Sequence, Cine Cameras, animation tracks, facial animation, audio, Niagara systems, environment dressing and final shots must be authored in Unreal Editor using the project's real assets. This separation is intentional: code controls state/lifecycle; Sequencer controls the authored film.

## Stage 1 connection
The opening ends after the hero's first awakening and escape. Stage 1 then becomes the canonical **return to the mines** rescue/revenge objective defined in `HERO_ORIGIN_AND_MAIN_STORY.md`. The opening must not incorrectly present the mine escape itself as the complete Stage 1 objective.

## Quality bar
The opening is not considered complete until:
1. The final real hero/character assets are used.
2. The final environment assets are dressed and lit.
3. The cinematic uses authored camera shots/Sequencer rather than only gameplay camera movement.
4. Facial/body animation is authored for the key beats.
5. Thunder, wind, dust and debris are synchronized.
6. Audio/music transitions are authored.
7. The title reveal and hand-off to gameplay are seamless.
8. Android scalability has a lower-cost cinematic path without changing the story.
