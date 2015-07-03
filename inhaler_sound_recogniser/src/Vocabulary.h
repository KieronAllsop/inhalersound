#pragma once

#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

typedef struct
{
	std::string Name;

	short int nStates;
	std::vector<int> PhysicalStates;

	short int nMixtures;

	unsigned int StartLogicalPosition;
	unsigned int EndLogicalPosition;
} VUnit;

class TVocabulary
{
public:
	TVocabulary(const std::string& vocName, const std::string& stateName, const std::string& mixName);
	~TVocabulary();

	std::vector<VUnit> Units;
	int nUnits;
    unsigned int nPhysicalStates;
    unsigned int nLogicalStates;

	// Ignored units (Filler)
	std::vector<int> IUnits;
	int nIUnits;
	// Cough units
	std::vector<int> CUnits;
	int nCUnits;

	// Minimum Durations in Milliseconds
	std::vector<int> MinDurations;

	// Returns the position of the unit with the specified name
	int GetUnitPositionByName(const std::string& name);

	// Return the unit to which the specified logical state belongs
	int GetUnitPositionByState(unsigned int logicalState);

	// Given a physical state, return the number of mixtures in the unit containing that physical state
	short int GetUnitNumMixturesByPhysicalState(unsigned int physicalState);

	// Return true if a given logical state belongs to an ignored unit
	bool IgnoreState(unsigned int logicalState);
	bool IgnoreUnitByName(const std::string& name);

	// Return true if a given logical state belongs to a cough unit
	bool CoughState(unsigned int logicalState);
	bool CoughUnitByName(const std::string& name);

	bool InUnitListByName(std::vector<int>& units, const std::string& name);

	int GetMinimumDurationByName(const std::string& name);

private:
	void LoadVocabulary(const std::string& fileName);
	void LoadStateStructure(const std::string& fileName);
	void LoadMixtureStructure(const std::string& fileName);

	bool InUnitList(std::vector<int>& units, int logicalState);
};

extern TVocabulary* Vocabulary;
