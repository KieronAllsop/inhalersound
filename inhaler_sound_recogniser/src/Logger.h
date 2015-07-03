#pragma once

// Adapted from: https://stackoverflow.com/questions/3662654/conditional-debug-output-class-with-templated-operator

#include <iostream>

#include "Global.h"

class Logger
{
public:
	Logger(LogLevel loggingLevel) : LoggingLevel(loggingLevel) { }

	template<typename T>
    Logger& operator<<(T t)
    {
        if(LoggingLevel <= GlobalSettings.LogLogLevel)
        {
            std::cout << t;
            return *this;
        }
        else
		{
            return *this;
		}
    }

	typedef std::ostream& (*STRFUNC)(std::ostream&);

	Logger& operator<<(STRFUNC func)
	{
		if(LoggingLevel <= GlobalSettings.LogLogLevel)
		{  
			func(std::cout);
		}
		return *this;
	}

	void flush()
	{
		std::cout.flush();
	}

private:
	LogLevel LoggingLevel;
};

