#pragma once

#include "GameFrameworkLib.h"
#include "GameUtilityLib.h"
#include "BaseCharacter.h"
#include "BaseAIController.h"
#include "AI/Navigation/NavigationPath.h"
#include "AI/Navigation/NavigationSystem.h"
#include "Navigation/PathFollowingComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"

namespace GameUtility
{
	class TakeCoverAction : public Action
	{
	public:
		inline TakeCoverAction(const UtilityParams& _params) : Action(_params) { }
		void onUpdate(Blackboard* _blackboard, Target* _target)
		{
			std::cout << "[TakeCoverAction] onUpdate" << std::endl;
			AActor* self = (AActor*)(_blackboard->getEntry("self"));
			assert(self != nullptr);

			ABaseCharacter* base = (ABaseCharacter*)self;
			assert(base != nullptr);

			base->SetActionTime(m_sName);
			//
		}
		inline static Action* create(const UtilityParams& _params) { return new TakeCoverAction(_params); }
	};
}