#pragma once

#include "GameGOAPLib.h"
#include "GameFrameworkLib.h"
#include "BaseCharacter.h"

namespace GameGOAP
{
	class EnemyDeadVariable : public Variable
	{
	public:
		EnemyDeadVariable(int _key, bool _value) : Variable(_key, _value) { }
		EnemyDeadVariable(int _key, bool _value, std::string& _name) :Variable(_key, _value, _name) { }
		EnemyDeadVariable(const Params& _params) : Variable(_params) { }
		EnemyDeadVariable(const EnemyDeadVariable& _variable) : Variable(_variable) { }
		inline static Variable* create(const Params& _params) { return new EnemyDeadVariable(_params); }
		void onUpdate(Blackboard* _blackboard)
		{
			std::cout << m_sName << " onUpdate" << std::endl;
			AActor* enemy = (AActor*)(_blackboard->getEntry("enemy"));
			assert(enemy != nullptr);

			ABaseCharacter* enemyBase = (ABaseCharacter*)enemy;
			m_bValue = enemyBase->IsDead();
		}
	};
}