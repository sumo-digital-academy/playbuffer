#pragma once

class Agent8; 

class State
{
public:
	virtual ~State() = default;
	virtual void Update( Agent8* player ) = 0;
};