#ifndef MoveToLastKnownPositionAction_h
#define MoveToLastKnownPositionAction_h

#include <stdio.h>
#include "GameBTLib.h"
#include "BaseCharacter.h"
#include "BaseAIController.h"
#include "GameFrameworkLib.h"
#include "../../Helpers/MathHelper.h"
#include "Math/Vector.h"
#include "AI/Navigation/NavigationPath.h"
#include "AI/Navigation/NavigationSystem.h"
#include "Navigation/PathFollowingComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"

using namespace GameBT;

class MoveToLastKnownPositionAction : public Behavior
{
public:
	MoveToLastKnownPositionAction() : Behavior("MoveToLastKnownPosition") { }
	inline virtual Status onUpdate(Blackboard* _blackboard)
	{
		AActor* self = (AActor*)(_blackboard->getEntry("self"));
		FVector currentLocation = self->GetActorLocation();
		ABaseCharacter* base = (ABaseCharacter*)self;
		ABaseAIController* ai = (ABaseAIController*)base->GetController();

		FVector* lastKnownPosition = (FVector*)(_blackboard->getEntry("lkp"));
		if (lastKnownPosition != nullptr)
		{
			EPathFollowingRequestResult::Type result = ai->MoveToLocation(*lastKnownPosition);
			if (result == EPathFollowingRequestResult::Type::AlreadyAtGoal)
			{
				m_eStatus = Status::BH_SUCCESS;
				return m_eStatus;
			}
		}
		else
		{
			m_eStatus = Status::BH_FAILURE;
			return m_eStatus;
		}

		m_eStatus = Status::BH_RUNNING;
		return m_eStatus;
	}
	inline static Behavior* create(const BehaviorParams& _params) { return new MoveToLastKnownPositionAction; }
};

#endif