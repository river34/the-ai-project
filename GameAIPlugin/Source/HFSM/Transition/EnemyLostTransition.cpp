#include "EnemyLostTransition.h"
#include "GameUtilityLib.h"
#include "GameMathLib.h"
#include "Runtime/Engine/Classes/GameFramework/Actor.h"

bool EnemyLostTransition::isValid(const Blackboard* _blackboard) const
{
	AActor* self = (AActor*)(_blackboard->getEntry("self"));
	float view = *(float*)(_blackboard->getEntry("view"));
	AActor* enemy = (AActor*)(_blackboard->getEntry("enemy"));
	GameMath::Vector3 selfPosition = GameMath::Vector3(self->GetActorLocation);
	GameMath::Quaternion selfRotation = GameMath::Quaternion(self->GetActorRotation);
	GameMath::Vector3 enemyPosition = GameMath::Vector3(enemy->GetActorLocation);
	GameMath::Vector3 forward = GameMath::Quaternion::rotate(selfRotation, GameMath::Vector3::unitZ());
	return enemyPosition.inViewOf(selfPosition, forward, view) == false;
}