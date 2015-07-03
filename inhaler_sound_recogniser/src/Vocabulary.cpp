#include "Vocabulary.h"

#include <sstream>

TVocabulary::TVocabulary(const std::string& vocName, const std::string& stateName, const std::string& mixName)
{
	try
	{
		LoadVocabulary(vocName);
		LoadStateStructure(stateName);
		LoadMixtureStructure(mixName);
	}
	catch (std::runtime_error e)
	{
		throw;
	}
}


TVocabulary::~TVocabulary()
{
}

void TVocabulary::LoadVocabulary(const std::string& fileName)
{
	std::ifstream infile (fileName);

	if (!infile)
		throw std::runtime_error("Vocabulary: specified Vocabulary file: \"" + fileName + "\" does not exist or is inaccessible");

	std::string line;

	if (infile.is_open())
	{
		IUnits = std::vector<int>();
		nIUnits = 0;
		CUnits = std::vector<int>();
		nCUnits = 0;
		MinDurations = std::vector<int>();

		while (std::getline(infile, line))
		{
			if (line.empty()) continue;
			if (line.substr(0, 2).compare("//") == 0) continue;

			if (line.substr(0, 3).compare("O-:") == 0)
			{
				// Optional units would be handled here
				continue;
			}

			if (line.substr(0, 3).compare("I-:") == 0)
			{
				// Ignored units
				std::string buffer;
				std::stringstream ss(line);
				std::vector<std::string> elements;
				while (ss >> buffer)
				{
					elements.push_back(buffer);
				}

				if (elements.size() <= 1) continue;

				for (int i = 1; i < elements.size(); i++)
				{
					int unitPos = GetUnitPositionByName(elements[i]);
					if (unitPos < 0)
					{
						continue;
					}
					else 
					{
						nIUnits++;
						IUnits.push_back(unitPos);
					}
				}

				continue;
			}

			if (line.substr(0, 7).compare("Cough-:") == 0)
			{
				// Cough units
				std::string buffer;
				std::stringstream ss(line);
				std::vector<std::string> elements;
				while (ss >> buffer)
				{
					elements.push_back(buffer);
				}

				if (elements.size() <= 1) continue;

				for (int i = 1; i < elements.size(); i++)
				{
					int unitPos = GetUnitPositionByName(elements[i]);
					if (unitPos < 0)
					{
						continue;
					}
					else 
					{
						nCUnits++;
						CUnits.push_back(unitPos);
					}
				}

				continue;
			}

			if (line.substr(0, 8).compare("MinDur-:") == 0)
			{
				// Minimum durations for each unit
				std::string buffer;
				std::stringstream ss(line);
				std::vector<std::string> elements;
				while (ss >> buffer)
				{
					elements.push_back(buffer);
				}

				if (elements.size() <= 1) continue;

				for (int i = 1; i < elements.size(); i++)
				{
					MinDurations[i - 1] = (std::stoi(elements[i]));
				}

				continue;
			}

			VUnit unit = VUnit();
			unit.Name = line;

			Units.push_back(unit);
			MinDurations.push_back(-1);
		}

		nUnits = Units.size();

		infile.close();
	}
}

void TVocabulary::LoadStateStructure(const std::string& fileName)
{
	std::ifstream infile (fileName);

	if (!infile)
		throw std::runtime_error("Vocabulary: specified Vocabulary State file: \"" + fileName + "\" does not exist or is inaccessible");

	std::string line;

	if (infile.is_open())
	{
		infile >> nPhysicalStates;

		while (!infile.eof())
		{
			std::string unitName;
			infile >> unitName;

			if (unitName.empty()) continue;

			int index = GetUnitPositionByName(unitName);
			VUnit* unit = &Units[index];

			infile >> unit->nStates;

			unit->PhysicalStates.resize(unit->nStates);

			for (int i = 0; i < unit->nStates; i++)
			{
				infile >> unit->PhysicalStates[i];
			}
		}

		infile.close();
	}

	int logicalStatesCount = 0;
	for (int i = 0; i < nUnits; i++)
	{
		Units[i].StartLogicalPosition = logicalStatesCount;
		Units[i].EndLogicalPosition = (logicalStatesCount += Units[i].nStates);
	}

	nLogicalStates = logicalStatesCount;
}

void TVocabulary::LoadMixtureStructure(const std::string& fileName)
{
	std::ifstream infile (fileName);

	if (!infile)
		throw std::runtime_error("Vocabulary: specified Vocabulary Mixture file: \"" + fileName + "\" does not exist or is inaccessible");

	std::string line;

	if (infile.is_open())
	{
		while (!infile.eof())
		{
			std::string unitName;
			infile >> unitName;

			if (unitName.empty()) continue;

			int index = GetUnitPositionByName(unitName);
			VUnit* unit = &Units[index];

			infile >> unit->nMixtures;
		}

		infile.close();
	}
}

int TVocabulary::GetUnitPositionByName(const std::string& name)
{
	for (int i = 0; i < Units.size(); i++)
	{
		if (Units[i].Name.compare(name) == 0)
			return i;
	}
	return -1;
}

bool 
TVocabulary::InUnitList(std::vector<int>& units, int logicalState)
{
	for(int i = 0; i < units.size(); i++)
	{
         if(logicalState >= Units[units[i]].StartLogicalPosition && 
			 logicalState < Units[units[i]].EndLogicalPosition)
		 {
         	return true;
		 }
	}
    return false;
}

bool 
TVocabulary::IgnoreState(unsigned int logicalState)
{
	return InUnitList(IUnits, logicalState);
}

bool 
TVocabulary::IgnoreUnitByName(const std::string& name)
{
	return IgnoreState(Units[GetUnitPositionByName(name)].StartLogicalPosition);
}

bool 
TVocabulary::CoughState(unsigned int logicalState)
{
	return InUnitList(CUnits, logicalState);
}

bool 
TVocabulary::CoughUnitByName(const std::string& name)
{
	return CoughState(Units[GetUnitPositionByName(name)].StartLogicalPosition);
}

bool 
TVocabulary::InUnitListByName(std::vector<int>& units, const std::string& name)
{
	int ind = GetUnitPositionByName(name);
	return (ind >= 0 && InUnitList(units, Units[ind].StartLogicalPosition));
}

int 
TVocabulary::GetUnitPositionByState(unsigned int logicalState)
{
    for(int n = 0; n < nUnits; n++) 
	{
		if(logicalState >= Units[n].StartLogicalPosition && logicalState < Units[n].EndLogicalPosition) 
		{
         	return n;
        }
    }
    return -1; // Not found
}

short int 
TVocabulary::GetUnitNumMixturesByPhysicalState(unsigned int physicalState)
{
	for (int n = 0; n < nUnits; n++) 
	{
		for (int i = 0; i < Units[n].nStates; i++)		{			if (Units[n].PhysicalStates[i] == physicalState)
			{
				return Units[n].nMixtures;
			}
		}
    }
    return -1;
}

int 
TVocabulary::GetMinimumDurationByName(const std::string& name)
{
	int index = GetUnitPositionByName(name);
	return (index > -1) ? MinDurations[GetUnitPositionByName(name)] : -1;
}
