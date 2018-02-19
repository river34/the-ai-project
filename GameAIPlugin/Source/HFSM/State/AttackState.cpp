#include "AttackState.h"

void AttackState::onEnter()
{
	std::cout << "enter " << m_sName << std::endl;
}

void AttackState::onUpdate(const float _deltaTime, const Blackboard* _blackboard)
{
	std::cout << "update " << m_sName << std::endl;
}

void AttackState::onExit()
{
	std::cout << "exit " << m_sName << std::endl;
}