#pragma once

#include <iostream>
#include "GameFrameworkLib.h"
#include "GameUtilityLib.h"
#include "BaseCharacter.h"

namespace GameUtility
{
	class DistanceInput : public Input
	{
	public:
		inline DistanceInput(const UtilityParams& _params) : Input(_params) { }
		const float getValue(const Blackboard* _blackboard, const Target* _target, const Action* _action) const
		{
			AActor* self = (AActor*)(_blackboard->getEntry("self"));
			assert(self != nullptr);

			AActor* enemy = (AActor*)(_target->getActor());
			if (enemy != nullptr)
			{
				FVector SelfLocation = self->GetActorLocation();
				FVector EnemyLocation = enemy->GetActorLocation();
				float distance = FVector::Dist(SelfLocation, EnemyLocation);
				float output = normalize(distance);
				//std::cout << "  [DistanceInput] value = " << output << std::endl;
				return output;
			}


			//std::cout << "  [DistanceInput] value = 0" << std::endl;
			return 0.f;
		}
		inline static Input* create(const UtilityParams& _params) { return new DistanceInput(_params); }
	};
}