#ifndef HaveLastKnownPositionCondition_h
#define HaveLastKnownPositionCondition_h

#include <stdio.h>
#include "GameBTLib.h"
#include "BaseCharacter.h"
#include "GameFrameworkLib.h"

using namespace GameBT;

class HaveLastKnownPositionCondition : public Behavior
{
public:
	HaveLastKnownPositionCondition() : Behavior("HaveLastKnownPosition") { }
	inline virtual Status onUpdate(Blackboard* _blackboard)
	{
		FVector* lastKnownPosition = (FVector*)(_blackboard->getEntry("lkp"));
		if (lastKnownPosition != nullptr && lastKnownPosition->IsZero() == false)
		{
			FString fstr = lastKnownPosition->ToString();
			std::string str(TCHAR_TO_UTF8(*fstr));
			std::cout << "lastKnownPosition: " << str << std::endl;
			return Status::BH_SUCCESS;
		}

		return Status::BH_FAILURE;
	}
	inline static Behavior* create(const BehaviorParams& _params) { return new HaveLastKnownPositionCondition; }
};

#endif