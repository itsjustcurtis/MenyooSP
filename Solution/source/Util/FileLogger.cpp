/*
* Menyoo PC - Grand Theft Auto V single-player trainer mod
* Copyright (C) 2019  MAFINS
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*/
#include "FileLogger.h"

#include "..\macros.h"

#include <fstream>
#include <iomanip>
#include <time.h>
#include "../Natives/natives.h"
#include "../Menu/Menu.h"
#include "../Menu/MenuConfig.h"
#include "../Memory/GTAmemory.h"

namespace ige
{
	const char* VersionString(eGameVersion version) //this is fucking awful. Look into xMacros to do it better. 
	{
		static const char* versionStrings[] = {
		"VER_1_0_335_2_STEAM",
		"VER_1_0_335_2_NOSTEAM",
		"VER_1_0_350_1_STEAM",
		"VER_1_0_350_2_NOSTEAM",
		"VER_1_0_372_2_STEAM",
		"VER_1_0_372_2_NOSTEAM",
		"VER_1_0_393_2_STEAM",
		"VER_1_0_393_2_NOSTEAM",
		"VER_1_0_393_4_STEAM",
		"VER_1_0_393_4_NOSTEAM",
		"VER_1_0_463_1_STEAM",
		"VER_1_0_463_1_NOSTEAM",
		"VER_1_0_505_2_STEAM",
		"VER_1_0_505_2_NOSTEAM",
		"VER_1_0_573_1_STEAM",
		"VER_1_0_573_1_NOSTEAM",
		"VER_1_0_617_1_STEAM",
		"VER_1_0_617_1_NOSTEAM",
		"VER_1_0_678_1_STEAM",
		"VER_1_0_678_1_NOSTEAM",
		"VER_1_0_757_2_STEAM",
		"VER_1_0_757_2_NOSTEAM",
		"VER_1_0_757_4_STEAM",
		"VER_1_0_757_4_NOSTEAM",
		"VER_1_0_791_2_STEAM",
		"VER_1_0_791_2_NOSTEAM",
		"VER_1_0_877_1_STEAM",
		"VER_1_0_877_1_NOSTEAM",
		"VER_1_0_944_2_STEAM",
		"VER_1_0_944_2_NOSTEAM",
		"VER_1_0_1011_1_STEAM",
		"VER_1_0_1011_1_NOSTEAM",
		"VER_1_0_1032_1_STEAM",
		"VER_1_0_1032_1_NOSTEAM",
		"VER_1_0_1103_2_STEAM",
		"VER_1_0_1103_2_NOSTEAM",
		"VER_1_0_1180_2_STEAM",
		"VER_1_0_1180_2_NOSTEAM",
		"VER_1_0_1290_1_STEAM",
		"VER_1_0_1290_1_NOSTEAM",
		"VER_1_0_1365_1_STEAM",
		"VER_1_0_1365_1_NOSTEAM",
		"VER_1_0_1493_0_STEAM",
		"VER_1_0_1493_0_NOSTEAM",
		"VER_1_0_1493_1_STEAM",
		"VER_1_0_1493_1_NOSTEAM",
		"VER_1_0_1604_0_STEAM",
		"VER_1_0_1604_0_NOSTEAM",
		"VER_1_0_1604_1_STEAM",
		"VER_1_0_1604_1_NOSTEAM",
		"VER_1_0_1737_0_STEAM",
		"VER_1_0_1737_0_NOSTEAM",
		"VER_1_0_1737_6_STEAM",
		"VER_1_0_1737_6_NOSTEAM",
		"VER_1_0_1868_0_STEAM",
		"VER_1_0_1868_0_NOSTEAM",
		"VER_1_0_1868_1_STEAM",
		"VER_1_0_1868_1_NOSTEAM",
		"VER_1_0_1868_4_EGS",
		"VER_1_0_2060_0_STEAM",
		"VER_1_0_2060_0_NOSTEAM",
		"VER_1_0_2060_1_STEAM",
		"VER_1_0_2060_1_NOSTEAM",
		"VER_1_0_2189_0_STEAM",
		"VER_1_0_2189_0_NOSTEAM",
		"VER_1_0_2215_0_STEAM",
		"VER_1_0_2215_0_NOSTEAM",
		"VER_1_0_2245_0_STEAM",
		"VER_1_0_2245_0_NOSTEAM",
		"VER_1_0_2372_0_STEAM",
		"VER_1_0_2372_0_NOSTEAM",
		"VER_1_0_2545_0_STEAM",
		"VER_1_0_2545_0_NOSTEAM",
		"VER_1_0_2612_1_STEAM",
		"VER_1_0_2612_1_NOSTEAM",
		"VER_1_0_2628_2_STEAM",
		"VER_1_0_2628_2_NOSTEAM",
		"VER_1_0_2699_0_STEAM",
		"VER_1_0_2699_0_NOSTEAM",
		"VER_1_0_2699_16",
		"VER_1_0_2802_0",
		"VER_1_0_2824_0",
		"VER_1_0_2845_0",
		"VER_1_0_2944_0",
		"VER_1_0_3028_0",
		"VER_1_0_3095_0",
		"VER_1_0_3179_0",
		"VER_1_0_3258_0",
		"VER_1_0_3274_0",
		"VER_1_0_3323_0",
		"VER_1_0_3337_0",
		"VER_1_0_3351_0",
		"VER_1_0_3407_0",
		"VER_1_0_3411_0",
		"VER_1_0_3442_0",
		"VER_1_0_812_8",
		"Unknown",
		"VER_1_0_812_9"
		"Unknown",
		};

		int idx = static_cast<int>(version);
		if (idx >= 0 && idx < static_cast<int>(eGameVersion::VER_SIZE))
			return versionStrings[idx];

		return "Unknown";
	}
	FileLogger menyooLogObject("menyooLog.txt");
	std::ofstream& myLog = menyooLogObject.myFile;

	FileLogger::FileLogger(std::string fname)
	{
		myFile.open(fname.c_str());

		if (myFile.is_open())
		{
			time_t now = time(0);
			tm t;
			localtime_s(&t, &now);

			myFile << "Menyoo " << MENYOO_CURRENT_VER_ << std::endl;
			myFile << "Gameversion " << ige::VersionString(GTAmemory::GetGameVersion()) << std::endl;
			//myFile << "Player Name: " << PLAYER::GET_PLAYER_NAME(-1) << std::endl;
			myFile << "Log file created " << std::setfill('0') << std::setw(2) << t.tm_mday << "/" << std::setfill('0') << std::setw(2) << (t.tm_mon + 1) << "/" << t.tm_year + 1900 << std::endl;
		}

	}

	FileLogger::~FileLogger()
	{
		if (myFile.is_open())
		{
			myFile << std::endl << std::endl;

			myFile.close();
		}

	}

	void addlog(LogType logType, std::string message, std::string filename, int loglevel)
	{
		if (static_cast<int>(logType) <= loglevel)
		{
			ige::myLog << logType << (loglevel >= 3 ? filename : "") << ": " << message << std::endl;
		}
	}

	//overloaded function to define default file and loglevels unless otherwise specified
	void addlog(LogType logType, std::string& message) {
		addlog(logType, message, "", g_loglevel);
	}
}

std::ofstream& operator<<(std::ofstream& stream, ige::LogType logType)
{
	time_t now = time(0);
	tm t;
	localtime_s(&t, &now);

	stream << "[" << std::setfill('0') << std::setw(2) << t.tm_hour << ":" << std::setfill('0') << std::setw(2) << t.tm_min << ":" << std::setfill('0') << std::setw(2) << t.tm_sec << "] ";

	switch (logType)
	{
	case ige::LogType::LOG_INIT: stream << "INIT - "; break;
	case ige::LogType::LOG_ERROR: stream << "ERROR - "; break;
	case ige::LogType::LOG_WARNING: stream << "WARNING - "; break;
	case ige::LogType::LOG_INFO: stream << "INFO - "; break;
	case ige::LogType::LOG_DEBUG: stream << "DEBUG - "; break;
	case ige::LogType::LOG_TRACE: stream << "TRACE - "; break;
	}

	return stream;
}

