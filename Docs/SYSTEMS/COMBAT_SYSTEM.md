# Combat System

## Design goals
- Skill-based third-person combat.
- Light/heavy attacks are state-driven and animation-ready.
- Abilities supplement weapons instead of replacing them.
- Enemy-specific defenses and openings should prevent button-mashing.

## Player combat loop
1. Detect target/context.
2. Enter attack state.
3. Play the appropriate animation montage in Blueprint.
4. Enable a short hit window.
5. Apply damage through the target's health component.
6. Resolve hit reaction, stagger or counter.
7. Exit attack state and permit the next action after cooldown.

## Weapons
- Sword: balanced reach and speed.
- Dagger: fast attacks and mobility.
- Spear: long reach and spacing.
- Hammer: slow, high-impact attacks and armor breaking.
- Thunder weapon: evolves as the story reveals the hero's heritage.

## Thunder integration
Thunder abilities can modify combat states, create openings, break defenses, interrupt enemies, or control groups. Costs remain data-driven through the ability component.

## Mobile requirements
- Avoid per-frame allocations.
- Prefer short collision windows over continuous expensive traces.
- Keep VFX complexity scalable by quality tier.
