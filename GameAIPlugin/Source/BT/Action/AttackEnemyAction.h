#ifndef AttackEnemyAction_h
#define AttackEnemyAction_h

#include <stdio.h>
#include "GameBTLib.h"
#include "BaseCharacter.h"
#include "BaseAIController.h"
#include "GameFrameworkLib.h"
#include "../../Helpers/MathHelper.h"
#include "Math/Vector.h"
#include "AI/Navigation/NavigationPath.h"
#include "AI/Navigation/NavigationSystem.h"
#include "Navigation/PathFollowingComponent.h"
#include "Kismet/KismetSystemLibrary.h"

using namespace GameBT;

class AttackEnemyAction : public Behavior
{
public:
	AttackEnemyAction() : Behavior("AttackEnemy") { }
	virtual Status onUpdate(Blackboard* _blackboard)
	{
		AActor* self = (AActor*)(_blackboard->getEntry("self"));
		ABaseCharacter* base = (ABaseCharacter*)self;
		AActor* enemy = (AActor*)(_blackboard->getEntry("enemy"));
		ABaseCharacter* baseEnemy = (ABaseCharacter*)enemy;
		baseEnemy->OnHit(base);

		return Status::BH_SUCCESS;
	}
	inline static Behavior* create(const BehaviorParams& _params) { return new AttackEnemyAction; }
};

#endif