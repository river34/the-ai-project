#ifndef TransitionEnemyLost_h
#define TransitionEnemyLost_h

#include <stdio.h>
#include <iostream>
#include "GameHFSMLib.h"
#include "GameFrameworkLib.h"
#include "GameMathLib.h"
#include "GameFramework/Actor.h"
#include "Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"
#include "../Helpers/MathHelper.h"

using namespace GameFSM;

class EnemyLostTransition : public Transition
{
public:
	EnemyLostTransition() : Transition("EnemyLost") { }
	EnemyLostTransition(const std::string& _name) : Transition(_name) { }
	~EnemyLostTransition() { }
	bool isValid(const Blackboard* _blackboard) const
	{
		AActor* self = (AActor*)(_blackboard->getEntry("self"));
		float view = *(float*)(_blackboard->getEntry("view"));
		AActor* enemy = (AActor*)(_blackboard->getEntry("enemy"));
		assert(self != nullptr && enemy != nullptr);

		FHitResult Hit;
		FVector StartLocation = self->GetActorLocation() + self->GetActorForwardVector() * 100.f;
		FVector EndLocation = enemy->GetActorLocation();
		FCollisionQueryParams CollisionParameters;
		if (self->GetWorld()->LineTraceSingleByChannel(Hit, StartLocation, EndLocation, ECollisionChannel::ECC_WorldDynamic, CollisionParameters))
		{
			DrawDebugLine(self->GetWorld(), StartLocation, EndLocation, FColor::Yellow, true, 1.f, 0, 1.f);

			AActor* other = Hit.GetActor();
			if (UKismetMathLibrary::EqualEqual_ObjectObject(enemy, other))
			{
				GameMath::Vector3 selfPosition = MathHelper::getVector3(self->GetActorLocation());
				GameMath::Vector3 forward = MathHelper::getVector3(self->GetActorForwardVector());
				GameMath::Vector3 enemyPosition = MathHelper::getVector3(enemy->GetActorLocation());
				return enemyPosition.inViewOf(selfPosition, forward, view) == false;
			}
		}

		return true;
	}
	inline static Transition* create(const std::string& _name) { return new EnemyLostTransition(_name); }
};

#endif
