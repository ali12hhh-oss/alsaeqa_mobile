# ALSAEQA — Combat, Weapon Progression & Companion Design

## Purpose
This document refines the canonical adventure without replacing the story. If any detail here conflicts with the canonical story record, `Docs/HERO_ORIGIN_AND_MAIN_STORY.md` remains authoritative.

## 1. Continuous progression, not stage-locked inventory
The 1–70 roadmap identifies major story discoveries and mastery milestones. It does **not** mean that a weapon or ability can only exist inside one numbered stage.

The hero begins with a simple, believable weapon and limited thunder power. During the continuous journey he can discover, acquire, repair, upgrade and master additional weapons and abilities through exploration, combat, rescues, ancient mechanisms, guardians, hidden locations and story events.

Once a weapon is obtained, it remains available according to the save/progression rules and can be switched with other owned weapons when gameplay permits.

A roadmap stage may therefore represent:
- first discovery of a weapon;
- a major upgrade or mastery trial;
- a new combat application;
- a story revelation connected to the weapon;
- or the guardian/location that explains its origin.

It must not artificially prevent normal use of previously acquired equipment.

## 2. Weapon evolution
The intended arc is:

**simple starting weapon → improved weapons → specialized weapons → multiple owned weapons → advanced weapon mastery → complete thunder arsenal.**

Weapons should feel earned. They should generally come from meaningful discoveries, defeated guardians, dangerous ruins, hidden forges, rescued locations or other justified story events rather than appearing as unexplained inventory rewards.

Different weapons should create different combat decisions. Examples include close-range power, fast attacks, ranged pressure, heavy stagger, elemental/thunder interaction, crowd control and environmental interaction. Exact weapon assignments remain subordinate to the canonical story and roadmap.

## 3. Combat density and variety
Combat is a recurring part of the journey, not an occasional feature reserved for milestone stages.

Encounters should be distributed naturally through exploration:
- patrols and ambushes;
- prison and rescue fights;
- enemy camps and guarded routes;
- creature encounters;
- pursuit sequences;
- elite enemies;
- mini-bosses;
- major bosses;
- environmental combat sequences;
- and revisits where enemies/world states have changed.

The goal is variety rather than constant fighting. Exploration, puzzles, traversal, discoveries, rescues and quiet character moments must break up combat.

## 4. Enemy evolution
Enemy strength and behavior should grow alongside the hero.

Early enemies should pressure the hero's limited equipment. Later enemies should introduce armor, shields, ranged attacks, coordinated groups, counters to predictable thunder use, special resistances, elite behaviors and larger creatures. Bosses should test abilities the player has actually learned.

Enemy progression should include more than higher health: new tactics, movement, attack patterns, defenses, reactions, formations and environmental use are preferred.

## 5. Companion rule — travel together
The permanent female companion is a core travelling partner. The default state is that she **follows and travels with the hero**.

She can assist in:
- combat;
- rescues;
- traversal;
- contextual puzzles;
- environmental interaction;
- dialogue and discoveries;
- mount participation when appropriate;
- and scripted cinematic moments.

She must not routinely wander away and run a separate autonomous adventure while the hero continues elsewhere. Her personal story is revealed **through the shared journey**: conversations, memories, discoveries, captures/rescues, family clues and events experienced with the hero.

Temporary separation is allowed only when justified by the story or gameplay, such as capture, injury, blocked passage or a short scripted event. It must lead back to reunion or a clearly authored consequence; it is not a second independent campaign.

## 6. Companion AI
The companion should maintain believable formation and navigation around the hero, avoid blocking traversal and combat, react to nearby threats, join attacks/support actions when appropriate, and recover safely after scripted moments.

Her AI should be context-sensitive rather than permanently attacking. In exploration she follows; in combat she supports; during rescue she participates; during dialogue she stays spatially connected; during cinematics she follows authored staging.

## 7. Rescues during the journey
Rescue is a recurring adventure activity. The hero and companion can encounter prisoners, workers, captured allies and other people whose rescue changes the world.

Rescues can lead to:
- temporary companions;
- information and clues;
- new routes;
- later settlements or safe locations;
- rewards and equipment opportunities;
- changed enemy presence;
- later dialogue and revisits.

Rescue outcomes must persist in Save/Load.

## 8. Mount and creature progression
Mounts should expand traversal and combat rather than replace the journey. Horses, beasts, ThunderBeast and FlyingBeast can enter the progression at justified points.

The ThunderBeast should eventually feel substantially different from ordinary mounts through storm-oriented traversal, combat and sensory abilities, but its strongest abilities should require progression and mastery rather than appearing immediately.

## 9. Player-facing progression philosophy
The player should feel three simultaneous curves:

1. **Hero:** from exhausted escaped slave to confident heir and master of thunder.
2. **Arsenal:** from one simple weapon to a flexible collection of specialized weapons.
3. **World threat:** from ordinary slavers and patrols to organized hunters, monsters, ancient guardians and major enemies capable of challenging the completed arsenal.

The companion's trust, skills and personal revelations should rise alongside these curves.

## 10. Non-negotiable implementation rule
No feature is considered complete merely because an enum, class, stage entry or document exists. Combat, weapons, enemy progression, companion behavior and rescues must work together in an actual playable continuous flow, with real animation/VFX/audio hooks, persistence and real authored assets where available.
