#pragma once

#include "GameGOAPLib.h"
#include "GameFrameworkLib.h"
#include "BaseCharacter.h"

namespace GameGOAP
{
	class EnemyLostVariable : public Variable
	{
	public:
		EnemyLostVariable(int _key, bool _value) : Variable(_key, _value) { }
		EnemyLostVariable(int _key, bool _value, std::string& _name) :Variable(_key, _value, _name) { }
		EnemyLostVariable(const Params& _params) : Variable(_params) { }
		EnemyLostVariable(const EnemyLostVariable& _variable) : Variable(_variable) { }
		inline static Variable* create(const Params& _params) { return new EnemyLostVariable(_params); }
		void onUpdate(Blackboard* _blackboard)
		{
			std::cout << m_sName << " onUpdate" << std::endl;
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
				if (UKismetMathLibrary::EqualEqual_ObjectObject(enemy, other))
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

						m_bValue = false;
						return;
					}
				}
			}
			m_bValue = true;
		}
	};
}