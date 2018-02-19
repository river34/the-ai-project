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
	class AttackAction : public Action
	{
	public:
		inline AttackAction(const UtilityParams& _params) : Action(_params) { }
		void onUpdate(Blackboard* _blackboard, Target* _target)
		{
			std::cout << "[AttackAction] onUpdate" << std::endl;
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

				FHitResult Hit;
				FVector StartLocation = self->GetActorLocation();
				FVector EndLocation = enemy->GetActorLocation();
				FCollisionQueryParams CollisionParameters;
				if (self->GetWorld()->LineTraceSingleByChannel(Hit, StartLocation, EndLocation, ECollisionChannel::ECC_GameTraceChannel1, CollisionParameters))
				{
					DrawDebugLine(self->GetWorld(), StartLocation, EndLocation, FColor::Red, true, 1.f, 0, 1.f);

					AActor* other = Hit.GetActor();
					if (UKismetMathLibrary::EqualEqual_ObjectObject(enemy, other))
					{
						float distance = FVector::Dist(enemy->GetActorLocation(), self->GetActorLocation());
						if (distance < 200.f)
						{
							onMoveCompleted(_blackboard);
						}
					}
				}
			}
		}
		void onMoveCompleted(Blackboard* _blackboard)
		{
			AActor* self = (AActor*)(_blackboard->getEntry("self"));
			AActor* enemy = (AActor*)(_blackboard->getEntry("enemy"));
			assert(self != nullptr && enemy != nullptr);
			ABaseCharacter* selfBase = (ABaseCharacter*)self;
			ABaseCharacter* enemyBase = (ABaseCharacter*)enemy;
			assert(selfBase != nullptr && enemyBase != nullptr);
			enemyBase->OnHit(selfBase);
		}
		inline static Action* create(const UtilityParams& _params) { return new AttackAction(_params); }
	};
}
