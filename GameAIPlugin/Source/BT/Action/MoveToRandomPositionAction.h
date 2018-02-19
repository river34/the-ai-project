#ifndef MoveToRandomPositionAction_h
#define MoveToRandomPositionAction_h

#include <stdio.h>
#include <time.h>
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
#include "Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"

using namespace GameBT;

class MoveToRandomPositionAction : public Behavior
{
private:
	float m_fRange = 0.f;

public:
	MoveToRandomPositionAction() : Behavior("MoveToRandomPosition") { }
	MoveToRandomPositionAction(float _range) : m_fRange(_range), Behavior("MoveToRandomPosition") { }
	inline virtual Status onUpdate(Blackboard* _blackboard)
	{
		AActor* self = (AActor*)(_blackboard->getEntry("self"));
		FVector currentLocation = self->GetActorLocation();
		ABaseCharacter* base = (ABaseCharacter*)self;
		ABaseAIController* ai = (ABaseAIController*)base->GetController();

		FVector* target = (FVector*)(_blackboard->getEntry("target"));
		EPathFollowingRequestResult::Type result = ai->MoveToLocation(*target);
		if (result == EPathFollowingRequestResult::Type::AlreadyAtGoal)
		{
			onMoveCompleted(_blackboard);
		}

		m_eStatus = Status::BH_RUNNING;
		return m_eStatus;
	}
	void onMoveCompleted(Blackboard* _blackboard)
	{
		AActor* self = (AActor*)(_blackboard->getEntry("self"));
		FVector currentLocation = self->GetActorLocation();
		UNavigationSystem* NavSys = UNavigationSystem::GetCurrent(self->GetWorld());
		FNavLocation ResultLocation;
		float searchRange = (m_fRange >= 0.f) ? m_fRange : *((float*)_blackboard->getEntry("searchRange"));
		NavSys->GetRandomReachablePointInRadius(self->GetActorLocation(), searchRange, ResultLocation);
		FVector* target = (FVector*)(_blackboard->getEntry("target"));
		*target = FVector(ResultLocation.Location.X, ResultLocation.Location.Y, currentLocation.Z);
		UKismetSystemLibrary::DrawDebugSphere(self->GetWorld(), *target, 10.0f, 12, FColor(0, 255, 0), 10.f);

		// Log
		FString fstr = ResultLocation.Location.ToString();
		std::string str(TCHAR_TO_UTF8(*fstr));
		std::cout << "Create new target: " << str << std::endl;
	}
	inline static Behavior* create(const BehaviorParams& _params)
	{
		double range = 0;
		auto it = _params.find("range");
		if (it != _params.end())
		{
			std::stringstream s(it->second);
			s >> range;
		}
		return new MoveToRandomPositionAction(range);
	}
};

#endif