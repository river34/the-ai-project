#pragma once

#include <iostream>
#include "GameFrameworkLib.h"
#include "GameUtilityLib.h"
#include "BaseCharacter.h"

namespace GameUtility
{
	class HealthInput : public Input
	{
	public:
		inline HealthInput(const UtilityParams& _params) : Input(_params) { }
		const float getValue(const Blackboard* _blackboard, const Target* _target, const Action* _action) const
		{
			AActor* self = (AActor*)(_blackboard->getEntry("self"));
			assert(self != nullptr);

			ABaseCharacter* selfBase = (ABaseCharacter*)self;
			assert(selfBase != nullptr);

			float health = selfBase->GetHealth();
			float output = normalize(health);

			//std::cout << "  [HealthInput] value = " << output << std::endl;
			return output;
		}
		inline static Input* create(const UtilityParams& _params) { return new HealthInput(_params); }
	};
}