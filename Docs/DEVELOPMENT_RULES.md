# ALSAEQA — Development Rules

## Repository isolation
- This repository is the only canonical repository for **الصاعقة / ALSAEQA**.
- Game work must never be written to another repository.

## One continuous project
- ALSAEQA is one continuous game project.
- Do not create V1/V2/V3 editions or parallel project variants.
- New systems, content, fixes, optimization and improvements are added to this same project.

## Engineering principles
- Prefer reusable systems over one-off gameplay code.
- Keep gameplay data configurable rather than hard-coded where practical.
- Design every major system for Android scalability.
- Avoid making high-end rendering features mandatory for the baseline mobile profile.
- Keep C++ responsibilities clear and expose tunable gameplay behavior to Blueprints where useful.

## Content integrity
- Never add placeholder claims that imply a binary Unreal asset exists when it does not.
- `.uasset` and `.umap` files require Unreal Editor/content tooling; text-based specifications may define their intended structure until those assets are authored.
- All audio, visual and third-party content must have appropriate rights before shipping.

## Quality gates
Every substantial change should consider:
1. Build validity.
2. Runtime safety.
3. Android performance and memory.
4. Save/load compatibility.
5. Accessibility and input behavior.
6. Asset references and packaging.
7. Regression risk.
