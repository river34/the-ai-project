#pragma once

#include "GameFrameworkLib.h"
#include "GameUtilityLib.h"
#include "BaseCharacter.h"
#include "Distractor.h"
#include "BaseAIController.h"
#include "AI/Navigation/NavigationPath.h"
#include "AI/Navigation/NavigationSystem.h"
#include "Navigation/PathFollowingComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"

namespace GameUtility
{
	class MoveToEnemyAction : public Action
	{
	public:
		inline MoveToEnemyAction(const UtilityParams& _params) : Action(_params) { }
		void onUpdate(Blackboard* _blackboard, Target* _target)
		{
			std::cout << "[MoveToEnemyAction] onUpdate" << std::endl;
			AActor* self = (AActor*)(_blackboard->getEntry("self"));
			assert(self != nullptr);

			ABaseCharacter* base = (ABaseCharacter*)self;
			assert(base != nullptr);

			base->SetActionTime(m_sName);

			AActor* enemy = (AActor*)(_target->getActor());
			if (enemy != nullptr)
			{
				FVector currentLocation = self->GetActorLocation();
				ABaseCharacter* base = (ABaseCharacter*)self;
				ABaseAIController* ai = (ABaseAIController*)base->GetController();

				FVector target = enemy->GetActorLocation();
				EPathFollowingRequestResult::Type result = ai->MoveToLocation(target);

				//float distance = FVector::Dist(enemy->GetActorLocation(), self->GetActorLocation());
				//if (distance < 200.f)
				//{
				//	ADistractor* distractor = (ADistractor*)enemy;
				//	if (distractor != nullptr)
				//	{
				//		// it is a distractor
				//		// remove it from enemylist
				//		base->RemoveFromList(distractor);
				//	}
				//}
			}
		}
		inline static Action* create(const UtilityParams& _params) { return new MoveToEnemyAction(_params); }
	};
}
