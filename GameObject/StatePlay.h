#pragma once
#include "State.h"

class StatePlay : public State
{
public:
	~StatePlay() = default;
	void Update( Agent8* player ) override;
};