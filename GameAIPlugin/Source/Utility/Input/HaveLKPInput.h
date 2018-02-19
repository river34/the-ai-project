#pragma once

#include <iostream>
#include "GameFrameworkLib.h"
#include "GameUtilityLib.h"
#include "BaseCharacter.h"

namespace GameUtility
{
	class HaveLKPInput : public Input
	{
	public:
		inline HaveLKPInput(const UtilityParams& _params) : Input(_params) { }
		const float getValue(const Blackboard* _blackboard, const Target* _target, const Action* _action) const
		{
			FVector* lastKnownPosition = (FVector*)(_blackboard->getEntry("lkp"));
			if (lastKnownPosition != nullptr && lastKnownPosition->IsZero() == false)
			{
				//std::cout << "  [HaveLKPInput] value = 1" << std::endl;
				return 1.f;
			}
			//std::cout << "  [HaveLKPInput] value = 0" << std::endl;
			return 0.f;
		}
		inline static Input* create(const UtilityParams& _params) { return new HaveLKPInput(_params); }
	};
}