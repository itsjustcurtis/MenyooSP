/*
* Menyoo PC - Grand Theft Auto V single-player trainer mod
* Copyright (C) 2019  MAFINS
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*/
#include "WeatherOptions.h"

namespace sub
{
	void WeatherMenu()
	{
		bool spSnowOn = false;
		bool spSnowOff = false;
		bool windSpeedPlus = false;
		bool windSpeedMinus = false;
		bool rainFXPlus = false;
		bool rainFXMinus = false;
		bool wavesHeightPlus = false;
		bool wavesHeightMinus = false;
		bool gravityLevelPlus = false;
		bool gravityLevelMinus = false;
		bool clearWeatherOverride = false;

		float gravityMultiplier = GTAmemory::GetWorldGravity();
		float windSpeed = GET_WIND_SPEED();
		float wavesHeight = GET_DEEP_OCEAN_SCALER();

		AddTitle("Weather");
		AddTickol("Reset Weather", true, clearWeatherOverride, clearWeatherOverride, TICKOL::CROSS); 
		if (clearWeatherOverride)
		{
			World::ClearWeatherOverride();
		}
		for (auto& weatherName : World::sWeatherNames)
		{
			bool bWeatherPressed = false;
			AddTickol(weatherName.first, GET_PREV_WEATHER_TYPE_HASH_NAME() == GET_HASH_KEY(weatherName.second), bWeatherPressed, bWeatherPressed); 
			if (bWeatherPressed)
			{
				addlog(ige::LogType::LOG_DEBUG, "Setting weather to " + weatherName.first);
				World::SetWeatherOverride(weatherName.second);
			}
		}
		
		AddLocal("Snow On Terrain", g_spSnow.IsSnow(), spSnowOn, spSnowOff);
		AddNumber("Wind Speed", windSpeed, 2, null, windSpeedPlus, windSpeedMinus);
		AddNumber("Ocean Wave Strength", wavesHeight, 2, null, wavesHeightPlus, wavesHeightMinus);
		AddNumber("Rain Puddles Multiplier", g_rainFXIntensity, 2, null, rainFXPlus, rainFXMinus);
		AddNumber("Gravity", gravityMultiplier, 2, null, gravityLevelPlus, gravityLevelMinus);
		AddOption("Clouds", null, nullFunc, SUB::CLOUDOPS);
		AddOption("Water Hack (For Waves At Beaches)", null, nullFunc, SUB::WATERHACK);


		if (spSnowOn || spSnowOff) 
		{ 
			g_spSnow.ToggleSnow(spSnowOn); 
		}

		if (windSpeedPlus)
		{
			addlog(ige::LogType::LOG_TRACE, "windSpeed_plus");
			windSpeed += 0.1f;
			SET_WIND_SPEED(windSpeed);
		}
		if (windSpeedMinus)
		{
			addlog(ige::LogType::LOG_TRACE, "windSpeed_minus");
			windSpeed -= 0.1f;
			SET_WIND_SPEED(windSpeed);
		}

		if (wavesHeightPlus)
		{
			addlog(ige::LogType::LOG_TRACE, "wavesHeight_plus");
			wavesHeight += 0.1f;
			WATER::SET_DEEP_OCEAN_SCALER(wavesHeight);
		}

		if (wavesHeightMinus)
		{
			addlog(ige::LogType::LOG_TRACE, "wavesHeight_minus");
			wavesHeight -= 0.1f;
			WATER::SET_DEEP_OCEAN_SCALER(wavesHeight);
		}

		if (rainFXPlus)
		{
			addlog(ige::LogType::LOG_TRACE, "rainfxit_plus");
			if (g_rainFXIntensity < 45.0f) 
			{
				g_rainFXIntensity += 0.1f;
			}
			SET_RAIN(g_rainFXIntensity);
		}

		if (rainFXMinus)
		{
			addlog(ige::LogType::LOG_TRACE, "rainfxit_minus");
			if (g_rainFXIntensity > 0.0f) 
			{
				g_rainFXIntensity -= 0.1f;
			}
			SET_RAIN(g_rainFXIntensity);
		}

		if (gravityLevelPlus)
		{
			addlog(ige::LogType::LOG_TRACE, "gravityLevel_plus");
			gravityMultiplier += 0.1;
			GTAmemory::SetWorldGravity(gravityMultiplier);
		}

		if (gravityLevelMinus)
		{
			addlog(ige::LogType::LOG_TRACE, "gravityLevel_minus");
			gravityMultiplier -= 0.1;
			GTAmemory::SetWorldGravity(gravityMultiplier);
		}
	}

	namespace WeatherClouds
	{
		const std::array<std::string, 20> cloudNames
		{ {
			{ "Altostratus" },
			{ "Cirrocumulus" },
			{ "Cirrus" },
			{ "Clear 01" },
			{ "Cloudy 01" },
			{ "Contrails" },
			{ "Horizonband1" },
			{ "Horizonband2" },
			{ "Horizonband3" },
			{ "Horsey" },
			{ "Nimbus" },
			{ "NoClouds" },
			{ "Puffs" },
			{ "Rain" },
			{ "Shower" },
			{ "Snowy 01" },
			{ "Stormy 01" },
			{ "Stratoscumulus" },
			{ "Stripey" },
			{ "Wispy" }
			} };

		void CloudMenu()
		{
			AddTitle("Clouds");

			bool bResetPressed = false;
			AddTickol("Reset", true, bResetPressed, bResetPressed, TICKOL::CROSS); if (bResetPressed)
			{
				UNLOAD_ALL_CLOUD_HATS();
			}

			for (auto& name : cloudNames)
			{
				bool bPressed = false;
				AddOption(name, bPressed);
				if (bPressed)
				{
					addlog(ige::LogType::LOG_DEBUG, "Set Clouds: " + name);
					LOAD_CLOUD_HAT(name.c_str(), 0.5f);
				}
			}
		}
	}
}


#include "..\Menu\submenu_switch.h"
#include "..\Menu\submenu_enum.h"
REGISTER_SUBMENU(WEATHEROPS, 	sub::WeatherMenu)
REGISTER_SUBMENU(CLOUDOPS,   	sub::WeatherClouds::CloudMenu)