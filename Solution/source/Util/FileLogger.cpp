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
#include "GameVersionList.h"

#include "..\macros.h"

#include <fstream>
#include <iomanip>
#include <time.h>
#include "../Natives/natives.h"
#include "../Menu/Menu.h"
#include "../Menu/MenuConfig.h"
#include "../Memory/GTAmemory.h"

#define X(name, str) case name: return str;
#define XV(name, val, str) case name: return str;



namespace ige
{
	const char* VersionString(eGameVersion version)
	{
		switch (version)
		{
			GAME_VERSION_LIST
		default: return "Unknown";
		}
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

			myFile << "Menyoo Version: " << MENYOO_CURRENT_VER_ << std::endl;
			myFile << "Game Version: " << ((g_isEnhanced)?"Enhanced ":"Legacy ") << VersionString(static_cast<eGameVersion>(GTAmemory::GetGameVersion())) << std::endl;
			//myFile << "Player Name: " << PLAYER::GET_PLAYER_NAME(-1) << std::endl;
			myFile << "Date: " << std::setfill('0') << std::setw(2) << t.tm_mday << "/" << std::setfill('0') << std::setw(2) << (t.tm_mon + 1) << "/" << t.tm_year + 1900 << std::endl;
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

	void AddLogWithLocation(const char* file, int line, LogType logType, const std::string& message)
	{
		if (static_cast<int>(logType) <= g_loglevel)
		{
			const char* basename = strrchr(file, '\\');
			basename = basename ? basename + 1 : file;
			std::string location = g_loglevel >= 3 ? std::string(basename) + ":" + std::to_string(line) + " - " : "";
			ige::myLog << logType << location << message << std::endl;
		}
	}
}

#undef X
#undef XV

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

