#pragma once

#include <iostream>
#include "GameFrameworkLib.h"
#include "GameUtilityLib.h"
#include "BaseCharacter.h"

namespace GameUtility
{
	class EnemyHealthInput : public Input
	{
	public:
		inline EnemyHealthInput(const UtilityParams& _params) : Input(_params) { }
		const float getValue(const Blackboard* _blackboard, const Target* _target, const Action* _action) const
		{
			AActor* enemy = (AActor*)(_target->getActor());
			if (enemy != nullptr)
			{
				ABaseCharacter* enemyBase = (ABaseCharacter*)enemy;
				if (enemyBase != nullptr)
				{
					float health = enemyBase->GetHealth();
					float output = normalize(health);

					//std::cout << "  [EnemyHealthInput] value = " << output << std::endl;
					return output;
				}
			}

			//std::cout << "  [EnemyHealthInput] value = 0" << std::endl;
			return 0.f;
		}
		inline static Input* create(const UtilityParams& _params) { return new EnemyHealthInput(_params); }
	};
}