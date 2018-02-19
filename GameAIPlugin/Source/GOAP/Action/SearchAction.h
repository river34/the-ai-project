#pragma once

#include "GameGOAPLib.h"
#include "GameFrameworkLib.h"
#include "BaseCharacter.h"
#include "BaseAIController.h"
#include "AI/Navigation/NavigationPath.h"
#include "AI/Navigation/NavigationSystem.h"
#include "Navigation/PathFollowingComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"

namespace GameGOAP
{
	class SearchAction : public Action
	{
	public:
		SearchAction(const Params& _params) : Action(_params) { }
		SearchAction(const SearchAction& _action) : Action(_action) { }
		inline static Action* create(const Params& _params) { return new SearchAction(_params); }
		void onUpdate(Blackboard* _blackboard)
		{
			std::cout << m_sName << " onUpdate" << std::endl;
			AActor* self = (AActor*)(_blackboard->getEntry("self"));
			FVector currentLocation = self->GetActorLocation();
			ABaseCharacter* base = (ABaseCharacter*)self;
			ABaseAIController* ai = (ABaseAIController*)base->GetController();

			FVector* target = (FVector*)(_blackboard->getEntry("target"));
			EPathFollowingRequestResult::Type result = ai->MoveToLocation(*target);
			if (result == EPathFollowingRequestResult::Type::AlreadyAtGoal)
			{
				onMoveCompleted(_blackboard);
			}
		}
		void onMoveCompleted(Blackboard* _blackboard)
		{
			AActor* self = (AActor*)(_blackboard->getEntry("self"));
			FVector currentLocation = self->GetActorLocation();
			UNavigationSystem* NavSys = UNavigationSystem::GetCurrent(self->GetWorld());
			FNavLocation ResultLocation;
			float searchRange = *((float*)_blackboard->getEntry("searchRange"));
			NavSys->GetRandomReachablePointInRadius(self->GetActorLocation(), searchRange, ResultLocation);
			FVector* target = (FVector*)(_blackboard->getEntry("target"));
			*target = FVector(ResultLocation.Location.X, ResultLocation.Location.Y, currentLocation.Z);
			UKismetSystemLibrary::DrawDebugSphere(self->GetWorld(), *target, 10.0f, 12, FColor(0, 255, 0), 10.f);

			// Log
			FString fstr = ResultLocation.Location.ToString();
			std::string str(TCHAR_TO_UTF8(*fstr));
			std::cout << "Create new target: " << str << std::endl;
		}
	};
}