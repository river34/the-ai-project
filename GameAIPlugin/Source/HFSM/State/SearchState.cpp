#include "SearchState.h"

void SearchState::onEnter()
{
	std::cout << "enter " << m_sName << std::endl;
}

void SearchState::onUpdate(const float _deltaTime, const Blackboard* _blackboard)
{
	std::cout << "update " << m_sName << std::endl;
}

void SearchState::onExit()
{
	std::cout << "exit " << m_sName << std::endl;
}