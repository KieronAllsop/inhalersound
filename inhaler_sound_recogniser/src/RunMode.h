#pragma once

class IRunMode
{
public:
	IRunMode() {}
	virtual ~IRunMode() {}

	virtual void Run() = 0;
};

