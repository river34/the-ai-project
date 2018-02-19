#ifndef SearchState_h
#define SearchState_h

#include <stdio.h>
#include "GameHFSMLib.h"
#include "BaseCharacter.h"
#include "BaseAIController.h"
#include "AI/Navigation/NavigationPath.h"
#include "AI/Navigation/NavigationSystem.h"
#include "Navigation/PathFollowingComponent.h"

using namespace GameFSM;

class SearchState : public State
{
public:
	SearchState() : State("Search") { }
	SearchState(const std::string& _name) : State(_name) { }
	~SearchState() { }
	void onEnter() { std::cout << "enter " << m_sName << std::endl; }
	void onUpdate(const float _deltaTime, Blackboard* _blackboard)
	{ 
		// std::cout << "update " << m_sName << std::endl;

		AActor* self = (AActor*)(_blackboard->getEntry("self"));
		FVector currentLocation = self->GetActorLocation();
		ABaseCharacter* base = (ABaseCharacter*)self;
		ABaseAIController* ai = (ABaseAIController*)base->GetController();

		FVector* lastKnownPosition = (FVector*)(_blackboard->getEntry("lkp"));
		if (lastKnownPosition != nullptr)
		{
			EPathFollowingRequestResult::Type result = ai->MoveToLocation(*lastKnownPosition);
			if (result == EPathFollowingRequestResult::Type::AlreadyAtGoal)
			{
				onMoveCompleted(_blackboard);
			}
		}
		else
		{
			FVector* target = (FVector*)(_blackboard->getEntry("target"));
			EPathFollowingRequestResult::Type result = ai->MoveToLocation(*target);
			if (result == EPathFollowingRequestResult::Type::AlreadyAtGoal)
			{
				onMoveCompleted(_blackboard);
			}
		}
	}
	void onMoveCompleted(Blackboard* _blackboard)
	{
		AActor* self = (AActor*)(_blackboard->getEntry("self"));
		FVector currentLocation = self->GetActorLocation();
		UNavigationSystem* NavSys = UNavigationSystem::GetCurrent(self->GetWorld());
		FNavLocation ResultLocation;
		float searchRange = *((float*)_blackboard->getEntry("searchRange"));
		NavSys->GetRandomReachablePointInRadius(self->GetActorLocation(), searchRange, ResultLocation);
		FVector* target = (FVector*)(_blackboard->getEntry("target"));
		*target = FVector(ResultLocation.Location.X, ResultLocation.Location.Y, currentLocation.Z);
		UKismetSystemLibrary::DrawDebugSphere(self->GetWorld(), *target, 10.0f, 12, FColor(0, 255, 0), 10.f);

		// Log
		FString fstr = ResultLocation.Location.ToString();
		std::string str(TCHAR_TO_UTF8(*fstr));
		std::cout << "Create new target: " << str << std::endl;
	}
	void onExit(Blackboard* _blackboard)
	{ 
		std::cout << "exit " << m_sName << std::endl;

		// Stop movement
		AActor* self = (AActor*)(_blackboard->getEntry("self"));
		FVector currentLocation = self->GetActorLocation();
		ABaseCharacter* base = (ABaseCharacter*)self;
		ABaseAIController* ai = (ABaseAIController*)base->GetController();
		ai->StopMovement();
	}
	inline static State* create(const std::string& _name) { return new SearchState(_name); }
};

#endif