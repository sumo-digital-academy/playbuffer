#pragma once
#include "State.h"

class StateAppear : public State
{
public:
	~StateAppear() = default;
	void Update( Agent8* player ) override;
};