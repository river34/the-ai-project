#include "PatrolState.h"

void PatrolState::onEnter()
{
	std::cout << "enter " << m_sName << std::endl;
}

void PatrolState::onUpdate(const float _deltaTime, const Blackboard* _blackboard)
{
	std::cout << "update " << m_sName << std::endl;
}

void PatrolState::onExit()
{
	std::cout << "exit " << m_sName << std::endl;
}