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
	class RunAction : public Action
	{
	public:
		inline RunAction(const UtilityParams& _params) : Action(_params) { }
		void onUpdate(Blackboard* _blackboard, Target* _target)
		{
			std::cout << "[RunAction] onUpdate" << std::endl;
			AActor* self = (AActor*)(_blackboard->getEntry("self"));
			assert(self != nullptr);

			ABaseCharacter* base = (ABaseCharacter*)self;
			assert(base != nullptr);

			base->SetActionTime(m_sName);

			FVector currentLocation = self->GetActorLocation();
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
			AActor* enemy = (AActor*)(_blackboard->getEntry("enemy"));
			FVector currentLocation = self->GetActorLocation();
			UNavigationSystem* NavSys = UNavigationSystem::GetCurrent(self->GetWorld());
			FNavLocation ResultLocation;
			FVector dir = currentLocation - enemy->GetActorLocation();
			float saveDistance = *((float*)_blackboard->getEntry("saveDistance"));
			dir *= saveDistance;
			float searchRange = *((float*)_blackboard->getEntry("searchRange"));
			NavSys->GetRandomReachablePointInRadius(dir, searchRange, ResultLocation);
			FVector* target = (FVector*)(_blackboard->getEntry("target"));
			*target = FVector(ResultLocation.Location.X, ResultLocation.Location.Y, currentLocation.Z);
			UKismetSystemLibrary::DrawDebugSphere(self->GetWorld(), *target, 10.0f, 12, FColor(0, 255, 0), 10.f);

			// Log
			FString fstr = ResultLocation.Location.ToString();
			std::string str(TCHAR_TO_UTF8(*fstr));
			std::cout << "Create new target: " << str << std::endl;
		}
		inline static Action* create(const UtilityParams& _params) { return new RunAction(_params); }
	};
}