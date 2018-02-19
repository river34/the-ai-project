#pragma once

#include "GameFrameworkLib.h"
#include "GameUtilityLib.h"
#include "BaseCharacter.h"
#include "BaseAIController.h"
#include "AI/Navigation/NavigationPath.h"
#include "AI/Navigation/NavigationSystem.h"
#include "Navigation/PathFollowingComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"

namespace GameUtility
{
	class InvestigateAction : public Action
	{
	public:
		inline InvestigateAction(const UtilityParams& _params) : Action(_params) { }
		void onUpdate(Blackboard* _blackboard, Target* _target)
		{
			std::cout << "[InvestigateAction] onUpdate" << std::endl;
			AActor* self = (AActor*)(_blackboard->getEntry("self"));
			assert(self != nullptr);

			ABaseCharacter* base = (ABaseCharacter*)self;
			assert(base != nullptr);

			base->SetActionTime(m_sName);

			FVector currentLocation = self->GetActorLocation();
			ABaseAIController* ai = (ABaseAIController*)base->GetController();

			FVector* lastKnownPosition = (FVector*)(_blackboard->getEntry("lkp"));
			if (lastKnownPosition != nullptr)
			{
				EPathFollowingRequestResult::Type result = ai->MoveToLocation(*lastKnownPosition);
			}
		}
		inline static Action* create(const UtilityParams& _params) { return new InvestigateAction(_params); }
	};
}