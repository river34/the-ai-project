#ifndef MoveToEnemyAction_h
#define MoveToEnemyAction_h

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

class MoveToEnemyAction : public Behavior
{
public:
	MoveToEnemyAction() : Behavior("MoveToEnemy") { }
	inline virtual Status onUpdate(Blackboard* _blackboard)
	{
		AActor* self = (AActor*)(_blackboard->getEntry("self"));
		FVector currentLocation = self->GetActorLocation();
		ABaseCharacter* base = (ABaseCharacter*)self;
		ABaseAIController* ai = (ABaseAIController*)base->GetController();
		AActor* enemy = (AActor*)(_blackboard->getEntry("enemy"));

		EPathFollowingRequestResult::Type result = ai->MoveToActor(enemy);

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
				if (distance < 100.f)
				{
					return Status::BH_SUCCESS;
				}
			}
		}

		return Status::BH_RUNNING;
	}
	inline static Behavior* create(const BehaviorParams& _params) { return new MoveToEnemyAction; }
};

#endif