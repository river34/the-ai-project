#pragma once

#include <iostream>
#include "GameFrameworkLib.h"
#include "GameUtilityLib.h"
#include "BaseCharacter.h"
#include "GameMathLib.h"
#include "GameFramework/Actor.h"
#include "Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"
#include "../../Helpers/MathHelper.h"
#include "Kismet/KismetSystemLibrary.h"

namespace GameUtility
{
	class HitByEnemyInput : public Input
	{
	public:
		inline HitByEnemyInput(const UtilityParams& _params) : Input(_params) { }
		const float getValue(const Blackboard* _blackboard, const Target* _target, const Action* _action) const
		{
			AActor* self = (AActor*)(_blackboard->getEntry("self"));
			assert(self != nullptr);

			ABaseCharacter* base = (ABaseCharacter*)self;
			assert(base != nullptr);

			AActor* enemy = (AActor*)(_target->getActor());
			if (enemy != nullptr)
			{
				ABaseCharacter* enemyBase = (ABaseCharacter*)enemy;
				if (enemyBase != nullptr)
				{
					ABaseCharacter::LastHit lastHit = base->GetLastHitInfo();
					if (lastHit.other != nullptr)
					{
						if (lastHit.other->GetUniqueID() == enemyBase->GetUniqueID())
						{
							float output = normalize(self->GetWorld()->GetTimeSeconds() - lastHit.time);
							//std::cout << "    [HitByEnemyInput] world time = " << self->GetWorld()->GetTimeSeconds() << std::endl;
							//std::cout << "    [HitByEnemyInput] time = " << lastHit.time << std::endl;
							//std::cout << "  [HitByEnemyInput] value = " << output << std::endl;
							return output;
						}
					}
				}
			}

			//std::cout << "  [HitByEnemyInput] value = 0" << std::endl;
			return 1.f;
		}
		inline static Input* create(const UtilityParams& _params) { return new HitByEnemyInput(_params); }
	};
}