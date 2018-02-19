#pragma once

#include <iostream>
#include "GameFrameworkLib.h"
#include "GameUtilityLib.h"
#include "BaseCharacter.h"
#include "GameMathLib.h"
#include "GameFramework/Actor.h"
#include "Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"
#include "../../Helpers/MathHelper.h"
#include "Kismet/KismetSystemLibrary.h"

namespace GameUtility
{
	class SeeEnemyInput : public Input
	{
	public:
		inline SeeEnemyInput(const UtilityParams& _params) : Input(_params) { }
		const float getValue(const Blackboard* _blackboard, const Target* _target, const Action* _action) const
		{
			AActor* self = (AActor*)(_blackboard->getEntry("self"));
			assert(self != nullptr);

			ABaseCharacter* base = (ABaseCharacter*)self;
			assert(base != nullptr);

			float view = base->GetView();

			AActor* enemy = (AActor*)(_target->getActor());
			//std::cout << "    [SeeEnemyInput] target id = " << enemy->GetUniqueID() << std::endl;

			if (enemy != nullptr)
			{
				FHitResult Hit;
				FVector StartLocation = self->GetActorLocation();
				FVector EndLocation = enemy->GetActorLocation();
				FCollisionQueryParams CollisionParameters;
				if (self->GetWorld()->LineTraceSingleByChannel(Hit, StartLocation, EndLocation, ECollisionChannel::ECC_GameTraceChannel1, CollisionParameters))
				{
					DrawDebugLine(self->GetWorld(), StartLocation, EndLocation, FColor::Green, true, 1.f, 0, 1.f);

					AActor* other = Hit.GetActor();
					if (other != nullptr)
					{
						if (enemy->GetUniqueID() == other->GetUniqueID())
						{
							GameMath::Vector3 selfPosition = MathHelper::getVector3(self->GetActorLocation());
							GameMath::Vector3 forward = MathHelper::getVector3(self->GetActorForwardVector());
							GameMath::Vector3 enemyPosition = MathHelper::getVector3(enemy->GetActorLocation());

							if (enemyPosition.inFrontOf(selfPosition, forward))
							{
								FVector* lastKnownPosition = (FVector*)(_blackboard->getEntry("lkp"));
								if (lastKnownPosition == nullptr)
								{
									lastKnownPosition = new FVector;
								}
								*lastKnownPosition = enemy->GetActorLocation();

								UKismetSystemLibrary::DrawDebugSphere(self->GetWorld(), enemy->GetActorLocation(), 10.0f, 12, FColor(255, 0, 0), 10.f);

								//std::cout << "  [SeeEnemyInput] value = 1" << std::endl;
								return 1.f;
							}
						}
					}
				}
			}

			//std::cout << "  [SeeEnemyInput] value = 0" << std::endl;
			return 0.f;
		}
		inline static Input* create(const UtilityParams& _params) { return new SeeEnemyInput(_params); }
	};
}