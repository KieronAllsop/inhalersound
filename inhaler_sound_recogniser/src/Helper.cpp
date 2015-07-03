#include "Helper.h"

#include "Global.h"

namespace Helper
{
	bool FileExists(const std::string& fileName)
	{
		struct stat buffer;
		return (stat (fileName.c_str(), &buffer) == 0); 
	}

	bool WillOverflow(double a, double b)
	{
		if ( (a < 0.0) == (b < 0.0)
			&& std::abs( b ) > std::numeric_limits<double>::max() - std::abs( a ) ) 
		{
			return true;
		}
		return false;
	}

	int MillisecondsToFrames(int ms)
	{
		if (ms <= 0) return 0;

		int nSamples = (GlobalSettings.DecodingSamplingRate / 1000) * ms;

		if (nSamples < GlobalSettings.SpeechAnalysisParamaters.FrmLength) 
			return 0;
		else
			return (nSamples - GlobalSettings.SpeechAnalysisParamaters.FrmLength) / GlobalSettings.SpeechAnalysisParamaters.Shift + 1;
	}

	unsigned long FramesToMilliseconds(int nFrames)
	{
		long nSamples = ((nFrames - 1) * GlobalSettings.SpeechAnalysisParamaters.Shift) + GlobalSettings.SpeechAnalysisParamaters.FrmLength;

		return (nSamples / (GlobalSettings.DecodingSamplingRate / 1000));
	}

	unsigned long SamplesToMilliseconds(int nSamples)
	{
		return (nSamples / (GlobalSettings.DecodingSamplingRate / 1000));
	}

	// Get current date/time, format is YYYY-MM-DD.HH:mm:ss
	std::string CurrentDateTime() 
	{
		time_t now = time(0);
		struct tm tstruct;
		char buf[80];
		tstruct = *localtime(&now);

		strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);

		return buf;
	}
}
