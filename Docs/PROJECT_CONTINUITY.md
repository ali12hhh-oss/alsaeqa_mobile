# ALSAEQA — Project Continuity / Canonical State

Canonical repository: ali12hhh-oss/alsaeqa
Project: الصاعقة / ALSAEQA
Engine: Unreal Engine 5
Primary target: Android
Architecture: C++ gameplay foundation + Blueprint/Sequencer authored presentation.
Rule: one continuous project; no V1/V2/V3 forks; no primitive placeholders as final art.

## Stage 1 — Chains in the Mine
Mandatory independent gates:
- RescueWorkers = 5 distinct workers
- DefeatSlavers = 1 designated Stage 1 slaver

Enemy-force standard:
- The mine must contain **at least 7 enemies/slavers**, so the hostile force is clearly larger than the five workers being rescued.
- Seven is the production minimum, not a requirement to kill seven enemies.
- Exactly one or more specifically configured Stage 1 objective slavers may report the `DefeatSlavers` gate, but the mandatory gate remains `DefeatSlavers = 1`.
- The remaining enemies are supporting guards, patrols, responders and pursuit/combat units that create pressure and escalation without becoming an extra progression gate.
- Enemy placement must be distributed across the mine rather than stacking the whole force in one room. The encounter rhythm should alternate between discovery/stealth pressure, rescue interaction, pursuit/combat, traversal and the final confrontation.

Final sequence: awakening aftermath → return to mine → discover surviving workers → rescue 1 → enemy response → rescue 2 → rescue 3 → escalation → rescue 4 → rescue 5 → defeat designated slaver → first mine-network clue → automatic Stage 1→2 transition.

Five workers use distinct authored methods: Break Chain, Open Cage, Release Lift, Cut Binding, Escort Out. Each has a stable WorkerId and optional RescueSequenceTag.
Each designated slaver has stable StageOneSlaverId. Death is authoritative and reports once.

## Stage 1 persistence — implemented
ALSAEQASaveGame stores RescuedStageOneWorkerIds and DefeatedStageOneSlaverIds.
ALSAEQASaveManager exposes record/query/count APIs.
A previously rescued worker reloads as rescued and cannot count again.
A previously defeated designated slaver reloads as dead and cannot count again.

## Opening cinematic / action cinematics / main menu — engineering pass (this session)
Audited against Docs/OPENING_CINEMATIC.md, Docs/MAIN_MENU_AND_TITLE_SCREEN_DESIGN.md and
Docs/MASTER_COMPLETION_SPEC.md §5. Finding: the C++ layer for all three existed only as
minimal lifecycle scaffolding (state flags + a generic time-dilation "play a moment"
call) with **zero authored cinematic content** — no Level Sequence, no cine cameras, no
facial/body animation, no VFX/audio sync, no 3D menu scene, no UMG layout. That part is
unchanged and cannot be produced from a text repository; it requires Unreal Editor and
the real asset packs (Docs/REAL_ART_ASSET_PIPELINE.md).

What changed in the C++ state/lifecycle layer:
- `ALSAEQACinematicDirector` now drives the opening as a real six-beat state machine
  (`EALSAEQAOpeningSequenceStage` / `FALSAEQAOpeningSequenceBeat`) timed exactly to
  OPENING_CINEMATIC.md's 0:00–0:45 breakdown, broadcasting `OnOpeningStageEntered` /
  `HandleOpeningStage` per beat so Sequencer/Blueprint has a concrete per-shot hook
  instead of one opaque "opening" call.
- Fixed a correctness bug: `bOpeningCinematicPlayed` used to be set true on **start**,
  contradicting OPENING_CINEMATIC.md's explicit "persist only on completion" rule.
  Completion is now only persisted from `FinishOpeningCinematic()`, via a new
  `bOpeningCinematicCompleted` field on `ALSAEQASaveGame` +
  `MarkOpeningCinematicCompleted/HasCompletedOpeningCinematic/ResetOpeningCinematicCompleted`
  on `ALSAEQASaveManager`.
- Added `SkipOpeningCinematic()` / `CanSkipOpeningCinematic()`, which refuses to skip
  out of the final hand-off beat (`bCriticalBoundary`) so the title reveal / Stage 1
  camera hand-off always completes cleanly.
- New `UALSAEQACinematicPresetLibrary` (BlueprintFunctionLibrary): tuned
  SlowMotionScale/Duration presets per `EALSAEQACinematicEvent`, covering every event
  named in MASTER_COMPLETION_SPEC §5 (added `LedgeCatch`, `SlidingSave`,
  `ClimbFailureRescue`, `CompanionRescue`, `BossImpact`, `OpeningCompleted` to the enum).
  `PlayActionMomentForEvent()` on the director is the one-line call gameplay code
  should use instead of hand-tuning slow-motion values per call site.
- `ALSAEQAMainMenuWidget.h` had literal escaped `\n` characters corrupting the file
  (would not compile) — fixed. Also fixed `OnNewJourneyClicked()`, which previously did
  nothing when a save already existed instead of calling the already-declared
  `RequestNewJourneyConfirmation()` Blueprint hook — the "New Journey" button was a
  dead end for any returning player.
- Removed a stray, misnamed dead placeholder function (`UALSAEQSAStageOneSaveManagerPlaceholder`)
  left in `ALSAEQASaveManager.cpp`.
- `ALSAEQAMainMenuSubsystem`/`Widget` were already reasonably solid (real save-data
  binding, five real menu actions, confirmation gate on New Journey) — left logic as-is
  beyond the bug fix above.

Not touched, still open exactly as documented: the actual authored opening film, the
living 3D main-menu scene, UMG art/layout, RTL text styling, and per-shot camera work
for the action-cinematic system. All of that is Unreal Editor + real-asset work.

## Current committed foundation
- 1–70 stage registry and progression validation.
- Automatic stage flow with delayed transition.
- Stage objective component with Stage 1 5+1 gates.
- Worker rescue interaction and five presentation methods.
- Stable Stage 1 worker/slaver persistence.
- Designated slaver death linkage.
- First Stage 1 clue unlock when both gates complete.
- Automatic Stage 1→2 transition and cinematic hooks.
- Forward interaction input (E).
- Data-driven opening cinematic (6 beats) with completion-on-finish persistence and skip handling.
- Tuned action-cinematic preset library covering all MASTER_COMPLETION_SPEC §5 event types.
- Main menu subsystem/widget wired to real save state, with New Journey confirmation flow fixed.
- Real-asset import/release documentation and primitive-fallback prohibition.

## Stage 1 production layer still requiring Unreal asset authoring
The remaining work that cannot be honestly completed through source-only repository edits is placement and wiring inside the actual Unreal level: five distributed worker locations, **at least seven distributed mine enemies**, real cages/chains/lift/bindings, rescue animations, camera/Sequencer shots, VFX/audio, and coordinated enemy encounter choreography. These require the real binary .uasset/.umap content and an Unreal build/editor environment.

## Next engineering queue
1. Author the actual opening Level Sequence / cine cameras / facial animation in editor, bound to the six `OnOpeningStageEntered` beats.
2. Build the living 3D main-menu scene (UMG + world) bound to `FALSAEQAMainMenuState`.
3. Stage 1 enemy escalation/encounter orchestration.
4. Data-drive later-stage objectives.
5. Animation-notify melee hit windows.
6. Real climbing/ledge traversal and IK fall rescue.
7. Wind/environment reactions.
8. Complete ThunderBeast Crossing/Sense/Storm Charge.
9. Companion AI/combat/rescue/persistence.
10. Dialogue/quest/event orchestration and Sequencer.
11. Broader save persistence for story/world/mount states.
12. Real asset mapping/import.
13. Android optimization and actual UE build verification.

## Continuity protocol
At every new chat: read this file, Docs/ALSAEQA_MASTER_PROJECT_BIBLE.md, Docs/DEVELOPMENT_RULES.md, inspect latest relevant commits/files, continue from the highest unfinished item, and never recreate completed systems. After meaningful implementation, update this file.

## Quality gate
No feature is done because a class or document exists. Runtime connection, failure safety, persistence implications, Android scalability and Blueprint/asset integration must be considered. Never claim a UE build succeeded without an actual build result.
