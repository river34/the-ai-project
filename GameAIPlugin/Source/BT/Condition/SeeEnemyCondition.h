#ifndef SeeEnemyCondition_h
#define SeeEnemyCondition_h

#include <stdio.h>
#include "GameBTLib.h"
#include "BaseCharacter.h"
#include "BaseAIController.h"
#include "GameFrameworkLib.h"
#include "GameMathLib.h"
#include "GameFramework/Actor.h"
#include "Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"
#include "../../Helpers/MathHelper.h"
#include "EngineGlobals.h"
#include "Engine/Engine.h"

using namespace GameBT;

class SeeEnemyCondition : public Behavior
{
public:
	SeeEnemyCondition() : Behavior("SeeEnemy") { }
	inline virtual Status onUpdate(Blackboard* _blackboard) 
	{ 
		AActor* self = (AActor*)(_blackboard->getEntry("self"));
		float view = *(float*)(_blackboard->getEntry("view"));
		AActor* enemy = (AActor*)(_blackboard->getEntry("enemy"));
		assert(self != nullptr && enemy != nullptr);

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
				FString fstr = other->GetActorLabel();
				std::string str(TCHAR_TO_UTF8(*fstr));
				std::cout << "raycast hid: " << str << std::endl;
			}
			else
			{
				std::cout << "raycast hid not actor" << std::endl;
			}
			if (UKismetMathLibrary::EqualEqual_ObjectObject(enemy, other))
			{
				GameMath::Vector3 selfPosition = MathHelper::getVector3(self->GetActorLocation());
				GameMath::Vector3 forward = MathHelper::getVector3(self->GetActorForwardVector());
				GameMath::Vector3 enemyPosition = MathHelper::getVector3(enemy->GetActorLocation());

				std::cout << "dot: " << (enemyPosition - selfPosition).dot(forward) << std::endl;

				if (enemyPosition.inFrontOf(selfPosition, forward))
				{
					FVector* lastKnownPosition = (FVector*)(_blackboard->getEntry("lkp"));
					if (lastKnownPosition == nullptr)
					{
						lastKnownPosition = new FVector;
					}
					*lastKnownPosition = enemy->GetActorLocation();

					UKismetSystemLibrary::DrawDebugSphere(self->GetWorld(), enemy->GetActorLocation(), 10.0f, 12, FColor(255, 0, 0), 10.f);

					// Log
					FString fstr = lastKnownPosition->ToString();
					std::string str(TCHAR_TO_UTF8(*fstr));
					std::cout << "Create new lkp: " << str << std::endl;

					m_eStatus = Status::BH_SUCCESS;
					return m_eStatus;
				}
			}
		}

		m_eStatus = Status::BH_FAILURE;
		return m_eStatus;
	}
	inline static Behavior* create(const BehaviorParams& _params) { return new SeeEnemyCondition; }
};

#endif