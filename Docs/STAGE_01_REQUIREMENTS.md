# ALSAEQA — Stage 1 Complete Specification

## Stage
**1 — Chains in the Mine**

Stage 1 is the complete playable introduction after the mine collapse and thunder awakening. It must feel like a real escape/rescue sequence, not a checklist.

## Mandatory progression gates

Stage 1 completes only when BOTH conditions are true:

1. **RescueWorkers = 5/5**
   - Five distinct mine workers/prisoners.
   - Each worker is a separate actor/identity.
   - Each worker counts exactly once.
   - The five workers use different authored rescue scenarios:
     - Break Chain
     - Open Cage
     - Release Lift
     - Cut Binding
     - Escort Out
   - Worker locations are distributed through the mine so the player must move through the environment.

2. **DefeatSlavers = 1/1**
   - The objective is reported only by a slaver explicitly configured as a Stage 1 objective enemy.
   - Death is the authoritative completion event.
   - The same enemy cannot report twice.

No other action can directly complete Stage 1.

## Story sequence

### A. Return after awakening
The hero has just survived the collapse, discovered the relic and awakened inherited thunder. He returns to the mine environment rather than immediately leaving it.

### B. Discover the prisoners
The player encounters evidence that other workers survived. The mine contains multiple separated groups/locations rather than one convenient prisoner.

### C. Rescue progression
The player moves through the mine and frees the five workers. Each rescue has its own presentation hook so Blueprint/Sequencer can provide the real animation, camera, sound and environmental response.

The HUD/progression layer should communicate:
**إنقاذ العمال 0/5 → 1/5 → … → 5/5**

### D. Enemy response
The slaver force reacts to the rescue operation. The player is not required to fight after every single rescue; encounters should be distributed so the sequence has rhythm:
- discovery/stealth pressure
- rescue interaction
- pursuit or combat response
- traversal to the next worker
- final confrontation

The enemy response must never reset completed worker rescues.

### E. Slaver defeat
At least one designated Stage 1 slaver must die. The death event reports DefeatSlavers +1 once.

### F. First clues
After the rescue/conflict sequence, the hero discovers the first story clues pointing toward the larger mystery. These are narrative discoveries, not a substitute for the two mandatory progression gates.

The clues should foreshadow:
- the organized force behind the mine/slaver network
- the existence of a larger conflict
- a connection to the hero's unexplained mark/relic
- the fact that the mine is only one part of a larger enemy operation

Do not reveal the complete Thunder Giants family truth in Stage 1.

## Cinematic beats

1. **Awakening aftermath** — short, restrained continuation from the opening.
2. **First worker discovered** — camera briefly establishes the scale of the mine and the prisoner situation.
3. **Major rescue** — selected worker receives an authored rescue shot.
4. **Enemy escalation** — after enough rescues, the mine becomes visibly/behaviorally more hostile.
5. **Final slaver death** — short impact moment; do not take control away for an unnecessarily long cinematic.
6. **First clues** — controlled story beat that raises questions.
7. **Stage transition** — automatic transition into Stage 2.

Cinematic events use the existing Rescue and StageTransition hooks. Real final camera/animation/audio content belongs in Blueprint/Sequencer and real assets.

## Player interaction

- Interaction is forward-facing and range-limited.
- Stage 1 worker interaction uses the existing E input mapping.
- A rescued worker cannot be counted again.
- Interacting with a worker outside Stage 1 cannot accidentally advance the Stage 1 objective.
- Invalid/null actors must fail safely.

## World-state expectations

After rescue:
- The worker remains rescued rather than being reset by the stage transition.
- The mine remains part of the connected world.
- Previously opened routes/doors/interaction states should be persistable when those systems are authored.
- The five workers are part of the stage state and should eventually be represented in save data.

## Quality requirements

Stage 1 is not considered complete until:
- C++ compiles in the target UE environment.
- The five-worker objective is runtime-connected.
- Slaver death is runtime-connected.
- Automatic stage transition is runtime-connected.
- Rescue and transition cinematic hooks are runtime-connected.
- No primitive geometry is presented as final art.
- Android scalability is considered.
- Save/load does not falsely replay or duplicate completed rescues.
- The actual mine level contains the authored worker/enemy placements once the Unreal binary assets are available.

## Current code contract

UALSAEQAStageObjectiveComponent:
- RescueWorkers requirement = 5
- DefeatSlavers requirement = 1

AALSAEQAWorkerPrisonerActor:
- reports RescueWorkers +1 once
- supports five distinct rescue presentation methods
- supports RescueSequenceTag
- fires the existing Rescue cinematic event

AALSAEQAEnemyCharacter:
- reports DefeatSlavers +1 only when bCountsAsStageOneSlaver is true
- reports only once on death

UALSAEQAStageFlowComponent:
- waits for all objectives
- advances automatically to Stage 2
- saves the new stage
- fires StageTransition

## Final Stage 1 flow

**Awakening → return to mine → discover workers → rescue worker 1 → mine response → rescue worker 2 → rescue worker 3 → escalation → rescue worker 4 → rescue worker 5 → designated slaver defeated → first clues → automatic Stage 1 completion → Stage 2 transition.**

The two mandatory objective gates remain independent and must both be satisfied.
