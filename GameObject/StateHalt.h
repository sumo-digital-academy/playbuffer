#pragma once
#include "State.h"

class StateHalt : public State
{
public:
	~StateHalt() = default;
	void Update( Agent8* player ) override;
};