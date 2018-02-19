#pragma once

#include <iostream>
#include "GameFrameworkLib.h"
#include "GameUtilityLib.h"
#include "BaseCharacter.h"

namespace GameUtility
{
	class TimeInput : public Input
	{
	public:
		inline TimeInput(const UtilityParams& _params) : Input(_params) { }
		const float getValue(const Blackboard* _blackboard, const Target* _target, const Action* _action) const
		{
			AActor* self = (AActor*)(_blackboard->getEntry("self"));
			assert(self != nullptr);

			ABaseCharacter* base = (ABaseCharacter*)self;
			assert(base != nullptr);

			float time = base->GetActionTime(_action->getName());
			float output = normalize(self->GetWorld()->GetTimeSeconds() - time);
			//std::cout << "  [TimeInput] value = " << output << std::endl;
			return output;
		}
		inline static Input* create(const UtilityParams& _params) { return new TimeInput(_params); }
	};
}