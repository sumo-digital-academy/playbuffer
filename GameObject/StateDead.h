#pragma once
#include "State.h"

class StateDead : public State
{
public:
	~StateDead() = default;
	void Update( Agent8* player ) override;
};

