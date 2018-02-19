#pragma once

#include <iostream>
#include "GameGOAPLib.h"
#include "GameFrameworkLib.h"
#include "BaseCharacter.h"

namespace GameGOAP
{
	class WorldStateInspector
	{
	public:
		static bool onUpdate(const Blackboard* _blackboard, const std::vector<std::string>& _variableMap, WorldState& _currState)
		{
			bool isChanged = false;
			for (auto name : _variableMap)
			{
				auto it = std::find(_variableMap.begin(), _variableMap.end(), name);
				if (it == _variableMap.end()) continue;

				int key = std::distance(_variableMap.begin(), it);

				if (name == "EnemyInRange")
				{
					AActor* self = (AActor*)(_blackboard->getEntry("self"));
					AActor* enemy = (AActor*)(_blackboard->getEntry("enemy"));
					assert(self != nullptr && enemy != nullptr);

					FVector SelfLocation = self->GetActorLocation();
					FVector EnemyLocation = enemy->GetActorLocation();
					float distance = FVector::Dist(SelfLocation, EnemyLocation);

					bool value = _currState.getVariable(key);



					isChanged = true;
					_currState.setVariable(key, true);
				}
			}
			return isChanged;
		}
	};
}