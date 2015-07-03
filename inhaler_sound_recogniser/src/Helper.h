#pragma once

#include <boost/filesystem.hpp>

#include <string>
#include <sys/stat.h>

namespace Helper
{
	bool FileExists(const std::string& fileName);
	bool WillOverflow(double a, double b);
	int MillisecondsToFrames(int ms);
	unsigned long FramesToMilliseconds(int nFrames);
	unsigned long SamplesToMilliseconds(int nSamples);
	std::string CurrentDateTime();
}

