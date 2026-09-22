#include "Cinematic/ALSAEQACinematicPresetLibrary.h"

FALSAEQACinematicRequest UALSAEQACinematicPresetLibrary::MakeRequestForEvent(EALSAEQACinematicEvent Event)
{
    FALSAEQACinematicRequest Request;
    Request.Event = Event;

    switch (Event)
    {
    case EALSAEQACinematicEvent::CombatFinisher:
        Request.SlowMotionScale = 0.15f; Request.Duration = 1.6f; break;
    case EALSAEQACinematicEvent::PerfectDodge:
        Request.SlowMotionScale = 0.20f; Request.Duration = 0.5f; break;
    case EALSAEQACinematicEvent::ParryCounter:
        Request.SlowMotionScale = 0.25f; Request.Duration = 0.6f; break;
    case EALSAEQACinematicEvent::AbilityImpact:
        Request.SlowMotionScale = 0.30f; Request.Duration = 0.8f; break;
    case EALSAEQACinematicEvent::NearDeath:
        Request.SlowMotionScale = 0.10f; Request.Duration = 1.2f; break;
    case EALSAEQACinematicEvent::Fall:
        Request.SlowMotionScale = 0.40f; Request.Duration = 0.5f; break;
    case EALSAEQACinematicEvent::LedgeCatch:
        Request.SlowMotionScale = 0.35f; Request.Duration = 0.45f; break;
    case EALSAEQACinematicEvent::SlidingSave:
        Request.SlowMotionScale = 0.35f; Request.Duration = 0.5f; break;
    case EALSAEQACinematicEvent::ClimbFailureRescue:
        Request.SlowMotionScale = 0.30f; Request.Duration = 0.9f; break;
    case EALSAEQACinematicEvent::CompanionRescue:
        Request.SlowMotionScale = 0.30f; Request.Duration = 1.4f; break;
    case EALSAEQACinematicEvent::Rescue:
        Request.SlowMotionScale = 0.35f; Request.Duration = 1.0f; break;
    case EALSAEQACinematicEvent::TraversalStunt:
        Request.SlowMotionScale = 0.40f; Request.Duration = 0.6f; break;
    case EALSAEQACinematicEvent::GiantSnakeEncounter:
        Request.SlowMotionScale = 0.35f; Request.Duration = 1.8f; break;
    case EALSAEQACinematicEvent::BossReveal:
        Request.SlowMotionScale = 0.45f; Request.Duration = 2.2f; break;
    case EALSAEQACinematicEvent::BossImpact:
        Request.SlowMotionScale = 0.20f; Request.Duration = 1.0f; break;
    case EALSAEQACinematicEvent::StageTransition:
        Request.SlowMotionScale = 1.0f; Request.Duration = 0.0f; Request.bUseCameraShake = false; break;
    default:
        break;
    }

    return Request;
}
