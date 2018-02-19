#ifndef PatrolState_h
#define PatrolState_h

#include <stdio.h>
#include "GameHFSMLib.h"
#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "BaseAIController.h"
#include "../../Helpers/MathHelper.h"
#include "Math/Vector.h"
#include "AI/Navigation/NavigationPath.h"
#include "AI/Navigation/NavigationSystem.h"
#include "Navigation/PathFollowingComponent.h"
#include "Kismet/KismetSystemLibrary.h"

using namespace GameFSM;

class PatrolState : public State
{
public:
	PatrolState() : State("Patrol") { }
	PatrolState(const std::string& _name) : State(_name) { }
	~PatrolState() { }
	void onEnter() { std::cout << "enter " << m_sName << std::endl; }
	void onUpdate(const float _deltaTime, Blackboard* _blackboard)
	{ 
		// std::cout << "update " << m_sName << std::endl;

		AActor* self = (AActor*)(_blackboard->getEntry("self"));
		FVector currentLocation = self->GetActorLocation();
		ABaseCharacter* base = (ABaseCharacter*)self;
		ABaseAIController* ai = (ABaseAIController*)base->GetController();

		// Move forward
		//float speed = *((float*)_blackboard->getEntry("speed"));
		//currentLocation += speed * _deltaTime * self->GetActorForwardVector();
		//self->SetActorLocation(currentLocation);

		// If not reach target pos, go to target pos
		// Else, find a new target pos on nav mesh
		FVector* target = (FVector*)(_blackboard->getEntry("target"));
		EPathFollowingRequestResult::Type result = ai->MoveToLocation(*target);
		if (result == EPathFollowingRequestResult::Type::AlreadyAtGoal)
		{
			onMoveCompleted(_blackboard);
		}

		//GameMath::Vector3 targetVec = MathHelper::getVector3(*target);
		//GameMath::Vector3 currentVec = MathHelper::getVector3(currentLocation);
		//float dist = (targetVec - currentVec).sqMagnitude();
		//if (dist > 30.f)
		//{
		//	std::cout << "target = " << targetVec.toString() << std::endl;
		//	std::cout << "current = " << currentVec.toString() << std::endl;
		//	std::cout << "dist = " << dist << std::endl;
		//	ai->MoveToLocation(*target);

		//	//UNavigationSystem* NavSys = UNavigationSystem::GetCurrent(self->GetWorld());
		//	//assert(NavSys != nullptr);
		//	//UNavigationPath* tpath = NavSys->FindPathToLocationSynchronously(self->GetWorld(), self->GetActorLocation(), *target);
		//	//if (tpath != NULL)
		//	//{
		//	//	for (int pointiter = 0; pointiter < tpath->PathPoints.Num(); pointiter++)
		//	//	{
		//	//		UKismetSystemLibrary::DrawDebugSphere(self->GetWorld(), tpath->PathPoints[pointiter], 10.0f, 12, FColor(255, 0, 0), 1.f);
		//	//	}

		//	//	FVector next = tpath->PathPoints[0];
		//	//	//FVector dir = next - currentLocation;
		//	//	//dir.Normalize();
		//	//	FVector2D dir2d = FVector2D(next.X - currentLocation.Y, next.X - currentLocation.Y);
		//	//	dir2d.Normalize();
		//	//	FVector dir = FVector(dir2d.X, dir2d.Y, 0);
		//	//	currentLocation += speed * _deltaTime * dir;
		//	//	self->SetActorLocation(currentLocation);

		//	//	// Log
		//	//	FString fstr = next.ToString();
		//	//	std::string str(TCHAR_TO_UTF8(*fstr));
		//	//	std::cout << "Move to " << str << std::endl;

		//	//	FString fstr2 = dir.ToString();
		//	//	std::string str2(TCHAR_TO_UTF8(*fstr2));
		//	//	std::cout << "Dir " << str2 << std::endl;
		//	//}
		//}
		//else
		//{
		//	UNavigationSystem* NavSys = UNavigationSystem::GetCurrent(self->GetWorld());
		//	FNavLocation ResultLocation;
		//	NavSys->GetRandomPoint(ResultLocation);
		//	*target = FVector(ResultLocation.Location.X, ResultLocation.Location.Y, currentLocation.Z);
		//	UKismetSystemLibrary::DrawDebugSphere(self->GetWorld(), *target, 10.0f, 12, FColor(0, 255, 0), 10.f);

		//	// Log
		//	FString fstr = ResultLocation.Location.ToString();
		//	std::string str(TCHAR_TO_UTF8(*fstr));
		//	std::cout << "Create new target: " << str << std::endl;
		//}

		// Play animation
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
	inline static State* create(const std::string& _name) { return new PatrolState(_name); }
};

#endif