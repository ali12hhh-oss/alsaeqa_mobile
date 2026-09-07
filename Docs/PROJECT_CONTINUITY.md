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
- Cinematic action moments and Blueprint story-beat hook.
- Real-asset import/release documentation and primitive-fallback prohibition.

## Stage 1 production layer still requiring Unreal asset authoring
The remaining work that cannot be honestly completed through source-only repository edits is placement and wiring inside the actual Unreal level: five distributed worker locations, **at least seven distributed mine enemies**, real cages/chains/lift/bindings, rescue animations, camera/Sequencer shots, VFX/audio, and coordinated enemy encounter choreography. These require the real binary .uasset/.umap content and an Unreal build/editor environment.

## Next engineering queue
1. Stage 1 enemy escalation/encounter orchestration.
2. Data-drive later-stage objectives.
3. Animation-notify melee hit windows.
4. Real climbing/ledge traversal and IK fall rescue.
5. Wind/environment reactions.
6. Complete ThunderBeast Crossing/Sense/Storm Charge.
7. Companion AI/combat/rescue/persistence.
8. Dialogue/quest/event orchestration and Sequencer.
9. Broader save persistence for story/world/mount states.
10. Real asset mapping/import.
11. Android optimization and actual UE build verification.

## Continuity protocol
At every new chat: read this file, Docs/ALSAEQA_MASTER_PROJECT_BIBLE.md, Docs/DEVELOPMENT_RULES.md, inspect latest relevant commits/files, continue from the highest unfinished item, and never recreate completed systems. After meaningful implementation, update this file.

## Quality gate
No feature is done because a class or document exists. Runtime connection, failure safety, persistence implications, Android scalability and Blueprint/asset integration must be considered. Never claim a UE build succeeded without an actual build result.
