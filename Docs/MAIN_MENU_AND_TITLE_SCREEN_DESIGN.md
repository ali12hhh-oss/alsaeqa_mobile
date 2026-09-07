# ALSAEQA — Professional Main Menu & Title Screen

## Status

**Canonical design requirement — September 2026**

This document defines the required main menu/title-screen experience for ALSAEQA. It is part of the single continuous canonical project and must be implemented as a real game system, not as a static mockup or decorative-only screen.

## 1. Core vision

The main screen must feel like entering the world of ALSAEQA, not opening a conventional application menu. The player should see a living cinematic 3D scene while menu controls remain clear and touch-friendly on Android.

The screen must avoid the traditional pattern of a flat background image plus a vertical list of generic buttons.

## 2. Living cinematic scene

The menu scene should contain, when the corresponding production assets are available:

- Alsaeqa in a real 3D environment.
- The permanent female companion after she has been rescued.
- Thunderstorm atmosphere, wind, clouds, rain/mist where appropriate, and controlled lightning activity.
- Ambient environmental motion so the world does not look frozen.
- Real character animation, idle behavior, clothing/hair/environment response where supported by the assets.
- Thunder/light interaction that reinforces the identity of the game.
- ThunderBeast or another owned mount in the background after its story/progression state makes it available.

No primitive geometry may be introduced as a final substitute for the user's real production assets.

## 3. Cinematic camera

The camera is part of the menu experience:

- Slow cinematic movement rather than a static camera.
- Subtle parallax and depth.
- Small camera emphasis when a menu item is selected.
- Controlled transitions between menu states.
- Mobile-safe framing for different aspect ratios.
- The camera must never obscure or interfere with touch controls.

## 4. Menu actions

The canonical primary actions are:

1. **متابعة الرحلة** — Continue Journey
   - Reads the real saved progression/checkpoint.
   - Shows useful context such as current stage/region when available.
   - Disabled or hidden when no valid save exists.
   - Loads the canonical saved journey rather than starting a parallel state.

2. **رحلة جديدة** — New Journey
   - Starts the canonical journey from the beginning.
   - Must use a confirmation step before replacing an existing save.
   - Opening cinematic hand-off must remain compatible with the canonical story.

3. **العالم** — World
   - Opens the real world/map/discovery interface when implemented.
   - Must respect discovered/revealed locations and progression.

4. **الإنجازات** — Achievements
   - Opens real achievement/progression information.
   - Must not be a decorative placeholder that reports fake completion.

5. **الإعدادات** — Settings
   - Opens actual game settings, including available audio, graphics and control options.

Android back behavior should be designed explicitly rather than adding a desktop-style Exit button solely for convention.

## 5. Progress-aware presentation

The main menu scene must be data-driven by the player's actual persistent state.

Examples:

- New game: Alsaeqa appears in his early survival state; the scene communicates the harsh beginning of the journey.
- Companion rescued: the permanent companion can appear with him in the scene.
- Mount progression unlocked: the appropriate owned mount/ThunderBeast can appear if its real asset and world integration are available.
- Later progression: environment, atmosphere, character state and contextual information may reflect the player's real progress.

These are presentation states derived from real save/progression data, not fake unlock counters.

## 6. Opening cinematic hand-off

The opening cinematic and main menu are one continuous presentation flow.

Required lifecycle:

`Launch -> Opening Cinematic (if required) -> Cinematic Completed -> Main Menu`

The cinematic system must distinguish **started** from **completed**. Completion should be persisted only when the sequence actually completes, so interruption/recovery cannot incorrectly mark the opening as finished.

The transition into the main menu should feel cinematic rather than like an abrupt level change.

## 7. UI language and presentation

The interface is primarily Arabic and must support proper RTL presentation. English/internal identifiers may remain in code, but player-facing Arabic labels must be correctly aligned and rendered.

Controls must be:

- Large enough for reliable touch interaction.
- Visually integrated with the world.
- Clear without covering the cinematic composition.
- Responsive with press, focus/selection and transition feedback.
- Suitable for Android performance budgets.

The design should use coordinated atmospheric colors/materials and should not depend on plain black/white panels as the main visual language.

## 8. Interaction feedback

Every actionable item should have a clear state:

- Normal.
- Focused/selected.
- Pressed.
- Disabled.
- Transitioning.

Feedback may include subtle scale, glow, movement, sound and camera emphasis. Effects must remain performant and must not become visual noise.

## 9. Audio

The menu should support a dedicated atmospheric audio layer and UI feedback sounds. Audio must respect the game's real local/production audio pipeline and must stop or transition correctly when leaving the menu.

No placeholder audio should be represented as final production content.

## 10. Save/progression integration

The main menu is a consumer of the canonical save/progression systems. It must not maintain a second independent progression model.

At minimum, the presentation layer must be able to determine:

- Whether a valid save exists.
- Current stage/checkpoint when available.
- Relevant region/world state.
- Permanent companion state.
- Mount/ThunderBeast availability.
- Major progression context needed for the menu scene.

If the underlying save system does not yet expose a required value, add a reusable accessor rather than duplicating the state in the menu.

## 11. Real asset policy

The final menu must use the project's real production assets supplied by the user. If a required asset is not yet present in the repository, implementation should provide a clean binding/interface/data slot for that asset rather than fabricating a primitive replacement and calling the feature complete.

Asset references must follow the project's real-art and licensing rules.

## 12. Architecture requirement

The implementation should be reusable and data-driven, with a separation between:

- Menu state/data.
- Save/progression queries.
- Cinematic/environment presentation.
- Camera direction.
- UI presentation.
- Navigation/actions.
- Audio feedback.

Blueprint/Sequencer hooks should be available for visual/cinematic authoring while core state and navigation remain testable and reusable in C++ where appropriate.

## 13. Acceptance criteria

The main menu is considered implemented only when it can, in the actual game build:

- Enter after the opening flow correctly.
- Display a living 3D presentation scene.
- Show the correct progress-aware state.
- Continue a real saved journey.
- Start a new journey safely.
- Open real world/achievement/settings systems when those systems exist.
- Provide proper Android touch feedback.
- Transition cleanly into and out of the game.
- Use production assets when available.
- Avoid primitive placeholders as a claimed final implementation.
- Preserve the single continuous canonical project architecture.

## 14. Relationship to existing canonical documents

This specification must be read together with:

- `Docs/HERO_ORIGIN_AND_MAIN_STORY.md` — story canon.
- `Docs/COMPLETE_ADVENTURE_STAGE_ROADMAP_1_70.md` — continuous progression.
- `Docs/MASTER_COMPLETION_SPEC.md` — completion/quality gate.
- `Docs/DEVELOPMENT_RULES.md` — canonical repository and real-asset rules.
- `Docs/COMBAT_PROGRESSION_AND_COMPANION_DESIGN.md` — companion and progression behavior.
- `Docs/OPENING_CINEMATIC.md` — opening presentation.

If a presentation idea conflicts with story or progression canon, the canonical story/progression documents take precedence and the menu must adapt to them.
