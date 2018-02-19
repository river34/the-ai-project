#ifndef ClearLastKnownPositionAction_h
#define ClearLastKnownPositionAction_h

#include <stdio.h>
#include "GameBTLib.h"
#include "BaseCharacter.h"
#include "GameFrameworkLib.h"

using namespace GameBT;

class ClearLastKnownPositionAction : public Behavior
{
public:
	ClearLastKnownPositionAction() : Behavior("ClearLastKnownPosition") { }
	inline virtual Status onUpdate(Blackboard* _blackboard)
	{
		FVector* lastKnownPosition = (FVector*)(_blackboard->getEntry("lkp"));
		if (lastKnownPosition != nullptr && lastKnownPosition->IsZero() == false)
		{
			*lastKnownPosition = FVector(0, 0, 0);
			return Status::BH_SUCCESS;
		}

		return Status::BH_FAILURE;
	}
	inline static Behavior* create(const BehaviorParams& _params) { return new ClearLastKnownPositionAction; }
};

#endif