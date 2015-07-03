#pragma once

#include <vector>
#include <string>

typedef struct 
{
	std::vector<float> Vector;
} AFrame;

typedef struct
{
	std::string Label;
	unsigned int BeginBoundary;
	unsigned int EndBoundary;
	float Score;
	bool Hit;
} PUnit;

struct PUnitLessThanScore
{
    inline bool operator() (const PUnit& unit1, const PUnit& unit2)
    {
		return (unit1.Score < unit2.Score);
    }
};

struct PUnitGreaterThanScore
{
    inline bool operator() (const PUnit& unit1, const PUnit& unit2)
    {
		return (unit1.Score > unit2.Score);
    }
};

struct ASegment
{
	std::vector<AFrame> Frames;
	unsigned int nFrames;
	std::vector<PUnit> Units;
	unsigned int nUnits;

	ASegment()
	{
		Frames = std::vector<AFrame>();
		nFrames = 0;
		Units = std::vector<PUnit>();
		nUnits = 0;
	}

};
