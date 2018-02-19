#ifndef WaitAction_h
#define WaitAction_h

#include <stdio.h>
#include <time.h>
#include "GameBTLib.h"

using namespace GameBT;

class WaitAction : public Behavior
{
private:
	double m_dDuration;
	time_t m_StartTime;

public:
	WaitAction(double _time) : m_dDuration(_time), Behavior("Wait") { }
	virtual void onInitialize(Blackboard* _blackboard) 
	{
		m_StartTime = time(NULL);
	}
	virtual Status onUpdate(Blackboard* _blackboard)
	{
		double seconds = difftime(time(NULL), m_StartTime);
		std::cout << "currTime = " << time(NULL) << std::endl;
		std::cout << "startTime = " << m_StartTime << std::endl;
		std::cout << "seconds = " << seconds << std::endl;
		if (seconds >= m_dDuration)
		{
			return Status::BH_SUCCESS;
		}
		return Status::BH_RUNNING;
	}
	static Behavior* create(const BehaviorParams& _params)
	{ 
		double time = 0;
		auto it = _params.find("time");
		if (it != _params.end())
		{
			std::stringstream s(it->second);
			s >> time;
		}
		return new WaitAction(time);
	}
};

#endif