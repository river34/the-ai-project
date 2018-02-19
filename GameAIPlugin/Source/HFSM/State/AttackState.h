#ifndef AttackState_h
#define AttackState_h

#include <stdio.h>
#include "GameHFSMLib.h"
#include "BaseCharacter.h"
#include "BaseAIController.h"
#include "../Helpers/MathHelper.h"
#include "Math/Vector.h"
#include "AI/Navigation/NavigationPath.h"
#include "AI/Navigation/NavigationSystem.h"
#include "Navigation/PathFollowingComponent.h"
#include "Kismet/KismetSystemLibrary.h"

using namespace GameFSM;

class AttackState : public State
{
public:
	AttackState() : State("Attack") { }
	AttackState(const std::string& _name) : State(_name) { }
	~AttackState() { }
	void onEnter()
	{ 
		std::cout << "enter " << m_sName << std::endl;
	}
	void onUpdate(const float _deltaTime, Blackboard* _blackboard)
	{ 
		// std::cout << "update " << m_sName << std::endl;
		
		AActor* self = (AActor*)(_blackboard->getEntry("self"));
		FVector currentLocation = self->GetActorLocation();
		ABaseCharacter* base = (ABaseCharacter*)self;
		ABaseAIController* ai = (ABaseAIController*)base->GetController();
		AActor* enemy = (AActor*)(_blackboard->getEntry("enemy"));

		EPathFollowingRequestResult::Type result = ai->MoveToActor(enemy);
		if (result == EPathFollowingRequestResult::Type::AlreadyAtGoal)
		{
			onMoveCompleted(_blackboard);
		}
	}
	void onMoveCompleted(Blackboard* _blackboard)
	{
		AActor* self = (AActor*)(_blackboard->getEntry("self"));
		ABaseCharacter* base = (ABaseCharacter*)self;
		AActor* enemy = (AActor*)(_blackboard->getEntry("enemy"));
		ABaseCharacter* baseEnemy = (ABaseCharacter*)enemy;
		baseEnemy->OnHit(base);
	}
	void onExit(Blackboard* _blackboard) { std::cout << "exit " << m_sName << std::endl; }
	inline static State* create(const std::string& _name) { return new AttackState(_name); }
};

#endif