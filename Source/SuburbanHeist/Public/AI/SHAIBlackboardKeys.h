#pragma once

#include "CoreMinimal.h"

/**
 * Blackboard key names shared between ASHResidentAIController and every custom BT node
 * (Section 8 suggested Blackboard variables). Configure BB_Resident with matching key types:
 *   TargetActor        - Object (Actor)
 *   LastHeardLocation   - Vector
 *   SuspicionLevel      - Float
 *   IsInvestigating      - Bool
 *   HasDetectedPlayer   - Bool
 *   DetectionState      - Enum (EResidentState)
 *   HomeLocation        - Vector
 * See Docs/BEHAVIOR_TREE_SPEC.md for the full BT_Resident node graph.
 */
namespace SHBlackboardKeys
{
	static const FName TargetActor(TEXT("TargetActor"));
	static const FName LastHeardLocation(TEXT("LastHeardLocation"));
	static const FName SuspicionLevel(TEXT("SuspicionLevel"));
	static const FName IsInvestigating(TEXT("IsInvestigating"));
	static const FName HasDetectedPlayer(TEXT("HasDetectedPlayer"));
	static const FName DetectionState(TEXT("DetectionState"));
	static const FName HomeLocation(TEXT("HomeLocation"));
}
