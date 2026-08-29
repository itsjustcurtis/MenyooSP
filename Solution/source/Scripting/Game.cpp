/*
* Menyoo PC - Grand Theft Auto V single-player trainer mod
* Copyright (C) 2019  MAFINS
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*/
#include "Game.h"

#include "..\macros.h"

#include "..\Util\GTAmath.h"
//#include "..\Scripting\enums.h"
#include "..\Natives\natives2.h"
#include "..\Memory\GTAmemory.h"
#include "..\Menu\Language.h"
#include "GTAentity.h"
#include "GTAped.h"
#include "GTAplayer.h"
#include "..\Menu\Menu.h"

#include <string>
#include <sstream>
#include <algorithm>
#include <optional>
#include <vector>
#include <Windows.h>
#include "../Util/FileLogger.h"
#include "../Util/ExePath.h"

std::ostream& operator<<(std::ostream& stream, std::wstring& text)
{
	stream << std::string(text.begin(), text.end());
	return stream;
}
std::wostream& operator<<(std::wostream& stream, std::string& text)
{
	stream << std::wstring(text.begin(), text.end());
	return stream;
}

namespace Game
{
	const std::pair<int, int> defaultScreenRes = { 1280, 720 };

	// Request asset
	bool RequestControlOfId(int netid)
	{
		NETWORK_REQUEST_CONTROL_OF_NETWORK_ID(netid);
		for (DWORD timeOut = GetTickCount() + 200; GetTickCount() < timeOut;)
		{
			if (NETWORK_HAS_CONTROL_OF_NETWORK_ID(netid))
				return true;
			WAIT(0);
		}
		return false;
	}
	bool RequestAnimDict(const std::string& anim_dict, DWORD timeOutms)
	{
		REQUEST_ANIM_DICT(anim_dict.c_str());
		for (DWORD timeOut = GetTickCount() + timeOutms; GetTickCount() < timeOut;)
		{
			if (HAS_ANIM_DICT_LOADED(anim_dict.c_str()))
				return true;
			WAIT(0);
		}
		return false;
	}
	bool RequestAnimSet(const std::string& anim_set, DWORD timeOutms)
	{
		REQUEST_ANIM_SET(anim_set.c_str());
		for (DWORD timeOut = GetTickCount() + timeOutms; GetTickCount() < timeOut;)
		{
			if (HAS_ANIM_SET_LOADED(anim_set.c_str()))
				return true;
			WAIT(0);
		}
		return false;
	}
	void RequestScript(const std::string& scriptName, int stackSize)
	{
		if (GET_NUMBER_OF_THREADS_RUNNING_THE_SCRIPT_WITH_THIS_HASH(GET_HASH_KEY(scriptName)) == 0 && DOES_SCRIPT_EXIST(scriptName.c_str()))
		{
			REQUEST_SCRIPT(scriptName.c_str());

			for (DWORD timeOut = GetTickCount() + 5000; GetTickCount() < timeOut;)
			{
				if (HAS_SCRIPT_LOADED(scriptName.c_str()))
					break;
				WAIT(0);
			}
			//while (!HAS_SCRIPT_LOADED(scriptName.c_str())) WAIT(0);

			START_NEW_SCRIPT(scriptName.c_str(), stackSize); // 1024 on console
			SET_SCRIPT_AS_NO_LONGER_NEEDED(scriptName.c_str());
		}
	}

	// GXT
	inline bool DoesGXTEntryExist(const std::string& entry)
	{
		return DOES_TEXT_LABEL_EXIST(entry.c_str()) != 0;
	}
	std::string GetGXTEntry(const std::string& entry, const std::string& fallback)
	{
		if (DoesGXTEntryExist(entry))
		{
			return GET_FILENAME_FOR_AUDIO_CONVERSATION(entry.c_str());
		}
		return fallback.empty() ? entry : fallback;
	}
	std::string GetGXTEntry(Hash entry, const std::string& fallback)
	{
		auto result = GTAmemory::GetGXTEntry(entry);
		return result == nullptr ? fallback : result;
	}

	namespace Sound
	{
		GameSound::GameSound()
			: active(false),
			soundID(-1)
		{
		}
		GameSound::GameSound(const std::string& nsoundSet, const std::string& nsound)
			: active(false),
			sound(nsound),
			soundSet(nsoundSet),
			soundID(-1)

		{
		}

		bool GameSound::LoadBank(const std::string& audioBank)
		{
			return REQUEST_SCRIPT_AUDIO_BANK((PCHAR)audioBank.c_str(), false, 0) != 0;
		}
		void GameSound::UnloadBank(const std::string& audioBank)
		{
			RELEASE_NAMED_SCRIPT_AUDIO_BANK((PCHAR)audioBank.c_str());
		}

		void GameSound::Play(GTAentity entity)
		{
			soundID = GET_SOUND_ID();
			PLAY_SOUND_FROM_ENTITY(soundID, (PCHAR)sound.c_str(), entity.Handle(), (PCHAR)soundSet.c_str(), 0, 0);
			active = true;
		}

		void GameSound::Stop()
		{
			if (soundID == -1 || !active) return;
			STOP_SOUND(soundID);
			active = false;
		}

		void GameSound::Destroy()
		{
			if (soundID == -1 || !active) return;
			RELEASE_SOUND_ID(soundID);
			soundID = -1;
			active = false;
		}

		void PlayFrontend(const std::string& sound_dict, const std::string& sound_name)
		{
			AUDIO::PLAY_SOUND_FRONTEND(-1, sound_name.c_str(), sound_dict.c_str(), FALSE);
		}
		void PlayFrontend_default(const std::string& sound_name)
		{
			AUDIO::PLAY_SOUND_FRONTEND(-1, sound_name.c_str(), "HUD_FRONTEND_DEFAULT_SOUNDSET", FALSE);
		}
	}

	namespace Print
	{
		// Game - Print/draw
		void setupdraw()
		{
			SET_TEXT_FONT(0);
			SET_TEXT_SCALE(0.4f, 0.4f);
			SET_TEXT_COLOUR(255, 255, 255, 255);
			SET_TEXT_WRAP(0.0f, 1.0f);
			SET_TEXT_CENTRE(0);
			SET_TEXT_DROPSHADOW(0, 0, 0, 0, 0);
			SET_TEXT_EDGE(0, 0, 0, 0, 0);
			//SET_TEXT_OUTLINE();
		}
		void SetupDraw(INT8 font, const Vector2& scale, bool centred, bool right_justified, bool outline, RGBA colour, Vector2 wrap)
		{
			SET_TEXT_FONT(font);
			SET_TEXT_SCALE(scale.x, scale.y);
			SET_TEXT_COLOUR(colour.R, colour.G, colour.B, colour.A);
			SET_TEXT_WRAP(wrap.x, wrap.y);
			SET_TEXT_RIGHT_JUSTIFY(right_justified);
			SET_TEXT_CENTRE(centred);
			SET_TEXT_DROPSHADOW(0, 0, 0, 0, 0);
			SET_TEXT_EDGE(0, 0, 0, 0, 0);
			if (outline) SET_TEXT_OUTLINE();
		}
		void drawstring(const std::string& s, float X, float Y)
		{
			if (s.length() < 100)
			{
				BEGIN_TEXT_COMMAND_DISPLAY_TEXT("STRING");
				ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME(s.c_str());
			}
			else
			{
				BEGIN_TEXT_COMMAND_DISPLAY_TEXT("jamyfafi");
				add_text_component_long_string(s);
			}
			END_TEXT_COMMAND_DISPLAY_TEXT(X, Y, 0);
		}
		void DrawString(std::ostream& os, float X, float Y)
		{
			const std::string& s = dynamic_cast<std::ostringstream&>(os).str();
			if (s.length() < 100)
			{
				BEGIN_TEXT_COMMAND_DISPLAY_TEXT("STRING");
				ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME(s.c_str());
			}
			else
			{
				BEGIN_TEXT_COMMAND_DISPLAY_TEXT("jamyfafi");
				add_text_component_long_string(s);
			}
			END_TEXT_COMMAND_DISPLAY_TEXT(X, Y, 0);
		}
		void drawstringGXT(const std::string& s, float X, float Y)
		{
			if (DOES_TEXT_LABEL_EXIST(s.c_str()))
			{
				BEGIN_TEXT_COMMAND_DISPLAY_TEXT(s.c_str());
				BEGIN_TEXT_COMMAND_SCALEFORM_STRING(s.c_str());
				END_TEXT_COMMAND_SCALEFORM_STRING();
			}
			else
			{
				if (s.length() < 100)
				{
					BEGIN_TEXT_COMMAND_DISPLAY_TEXT("STRING");
					ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME(s.c_str());
				}
				else
				{
					BEGIN_TEXT_COMMAND_DISPLAY_TEXT("jamyfafi");
					add_text_component_long_string(s);
				}
			}
			END_TEXT_COMMAND_DISPLAY_TEXT(X, Y, 0);
		}
		void drawstringGXT(std::ostream& os, float X, float Y)
		{
			const std::string& s = dynamic_cast<std::ostringstream&>(os).str();
			char* text = (char*)s.c_str();

			if (DOES_TEXT_LABEL_EXIST(text))
			{
				BEGIN_TEXT_COMMAND_DISPLAY_TEXT(text);
				BEGIN_TEXT_COMMAND_SCALEFORM_STRING(text);
				END_TEXT_COMMAND_SCALEFORM_STRING();
			}
			else
			{
				if (s.length() < 100)
				{
					BEGIN_TEXT_COMMAND_DISPLAY_TEXT("STRING");
					ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME(text);
				}
				else
				{
					BEGIN_TEXT_COMMAND_DISPLAY_TEXT("jamyfafi");
					add_text_component_long_string(s);
				}
			}
			END_TEXT_COMMAND_DISPLAY_TEXT(X, Y, 0);
		}
		void drawinteger(int text, float X, float Y)
		{
			BEGIN_TEXT_COMMAND_DISPLAY_TEXT("NUMBER");
			ADD_TEXT_COMPONENT_INTEGER(text);
			END_TEXT_COMMAND_DISPLAY_TEXT(X, Y, 0);
		}
		void drawfloat(double text, UINT8 decimal_places, float X, float Y)
		{
			if (decimal_places == 0 && text > 16777216.0)
			{
				BEGIN_TEXT_COMMAND_DISPLAY_TEXT("STRING");
				ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME(std::to_string(static_cast<long long>(text)).c_str());
				END_TEXT_COMMAND_DISPLAY_TEXT(X, Y, 0);
			}
			else
			{
				BEGIN_TEXT_COMMAND_DISPLAY_TEXT("NUMBER");
				ADD_TEXT_COMPONENT_FLOAT(static_cast<float>(text), decimal_places);
				END_TEXT_COMMAND_DISPLAY_TEXT(X, Y, 0);
			}
		}

		// Game::Print::PrintBottomCentre notifications
		namespace
		{
			struct PrintBottomCentreMessage
			{
				std::string messageText;
				DWORD expiresAt;
			};

			std::optional<PrintBottomCentreMessage> activePrintBottomCentreMessage;

			void DrawPrintBottomCentreMessage()
			{
				if (!activePrintBottomCentreMessage) return;

				const DWORD now = GetTickCount();
				if (now >= activePrintBottomCentreMessage->expiresAt)
				{
					activePrintBottomCentreMessage.reset();
					return;
				}

				SetupDraw(0, Vector2(0.4f, 0.4f), true, false, false);
				drawstring(activePrintBottomCentreMessage->messageText, 0.5f, 0.9f);
			}
		}

		void PrintBottomCentre(std::string s, int time)
		{
			activePrintBottomCentreMessage = PrintBottomCentreMessage{
				Language::TranslateToSelected(std::move(s)),
				GetTickCount() + static_cast<DWORD>((std::max)(0, time))
			};
		}

		void TickPrintBottomCentre()
		{
			DrawPrintBottomCentreMessage();
		}
		void PrintBottomCentre(std::ostream& s, int time)
		{
			PrintBottomCentre(dynamic_cast<std::ostringstream&>(s).str(), time);
		}
		void PrintBottomCentre(std::wostream& s, int time)
		{
			std::wstring wtext2 = (dynamic_cast<std::wostringstream&>(s).str());
			PrintBottomCentre(std::string(wtext2.begin(), wtext2.end()), time);
		}

		// Game Feed Notifications
		void GameFeedNotification::Hide()
		{
			THEFEED_REMOVE_ITEM(this->mHandle);
		}
		GameFeedNotification PrintBottomLeft(std::string s, bool gxt)
		{
			s = Language::TranslateToSelected(s);
			const char* text = s.c_str();

			if (gxt && DOES_TEXT_LABEL_EXIST(text))
				BEGIN_TEXT_COMMAND_THEFEED_POST(text);
			else
			{
				if (s.length() < 100)
				{
					BEGIN_TEXT_COMMAND_THEFEED_POST("STRING");
					ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME(text);
				}
				else
				{
					BEGIN_TEXT_COMMAND_THEFEED_POST("jamyfafi");
					add_text_component_long_string(s);
				}
			}

			Game::Sound::PlayFrontend("Phone_SoundSet_Default", "Text_Arrive_Tone");

			//END_TEXT_COMMAND_THEFEED_POST_TICKER_FORCED(0, 1);
			return END_TEXT_COMMAND_THEFEED_POST_TICKER(0, 0);
		}
		GameFeedNotification PrintBottomLeft(std::ostream& s, bool gxt)
		{
			return PrintBottomLeft((dynamic_cast<std::ostringstream&>(s).str()), gxt);
		}
		GameFeedNotification PrintBottomLeft(std::wostream& s, bool gxt)
		{
			std::wstring wtext = (dynamic_cast<std::wostringstream&>(s).str());
			return PrintBottomLeft(std::string(wtext.begin(), wtext.end()), gxt);
		}
		GameFeedNotification PrintBottomLeft(std::string s, const std::string& sender, const std::string& subject, const std::string& picName, int iconType, bool flash, bool gxt)
		{
			const char* text = s.c_str();

			if (gxt && DOES_TEXT_LABEL_EXIST(text))
				BEGIN_TEXT_COMMAND_THEFEED_POST(text);
			else
			{
				s = Language::TranslateToSelected(s);
				text = s.c_str();

				if (s.length() < 100)
				{
					BEGIN_TEXT_COMMAND_THEFEED_POST("STRING");
					ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME(text);
				}
				else
				{
					BEGIN_TEXT_COMMAND_THEFEED_POST("jamyfafi");
					add_text_component_long_string(s);
				}
			}

			END_TEXT_COMMAND_THEFEED_POST_MESSAGETEXT((PCHAR)picName.c_str(), (PCHAR)picName.c_str(), flash, iconType, (PCHAR)sender.c_str(), (PCHAR)subject.c_str());

			Game::Sound::PlayFrontend("Phone_SoundSet_Default", "Text_Arrive_Tone");

			return END_TEXT_COMMAND_THEFEED_POST_TICKER(0, 0);
		}
		GameFeedNotification PrintBottomLeft(std::ostream& s, const std::string& sender, const std::string& subject, const std::string& picName, int iconType, bool flash, bool gxt)
		{
			return PrintBottomLeft((dynamic_cast<std::ostringstream&>(s).str()), sender, subject, picName, iconType, flash, gxt);
		}
		GameFeedNotification PrintBottomLeft(std::wostream& s, const std::string& sender, const std::string& subject, const std::string& picName, int iconType, bool flash, bool gxt)
		{
			std::wstring wtext = (dynamic_cast<std::wostringstream&>(s).str());
			return PrintBottomLeft(std::string(wtext.begin(), wtext.end()), sender, subject, picName, iconType, flash, gxt);
		}

		// Custom notification system
		namespace
		{
			enum class NotificationAnimation { Entering, Visible, Exiting };

			struct QueuedNotification
			{
				std::optional<std::string> title;
				std::string description;
				std::vector<std::string> descriptionLines;
				float panelHeight = 0.0f;
				DWORD requestedDuration = 0;
				DWORD displayDuration = 0;
				DWORD visibleUntil = 0;
				DWORD animationStarted = 0;
				NotificationAnimation animation = NotificationAnimation::Entering;
			};

			std::optional<QueuedNotification> activeNotification;
			std::optional<QueuedNotification> pendingNotification;

			constexpr DWORD animationDuration = 220;
			constexpr float animationTravelDistance = 0.075f;
			constexpr float panelWidth = 0.27f;
			constexpr float stripeWidth = 0.003f;
			constexpr float leftPadding = 0.014f;
			constexpr float rightPadding = 0.012f;
			constexpr float topPadding = 0.012f;
			constexpr float bottomPadding = 0.012f;
			constexpr float titleDescriptionGap = 0.0f;
			constexpr float notificationBottomMargin = 0.06f;
			constexpr float titleLineHeight = 0.03f;
			constexpr float descriptionLineHeight = 0.02f;
			constexpr float titleTextScale = 0.40f;
			constexpr float descriptionTextScale = 0.33f;
			constexpr float textWidth = panelWidth - stripeWidth - leftPadding - rightPadding;
			constexpr size_t maximumTitleLength = 40;
			constexpr size_t maximumDescriptionLength = 300;

			bool IsFormattingTag(const std::string& tag)
			{
				return tag == "~r~" || tag == "~b~" || tag == "~g~" || tag == "~y~" || tag == "~p~" || tag == "~w~" || tag == "~o~" || tag == "~c~" || tag == "~m~" || tag == "~u~" || tag == "~s~" || tag == "~n~";
			}

			bool IsPersistentFormattingTag(const std::string& tag)
			{
				return IsFormattingTag(tag) && tag != "~s~" && tag != "~n~";
			}

			std::string GetVisibleText(const std::string& text)
			{
				std::string visible;
				for (std::string::size_type index = 0; index < text.length();)
				{
					if (text[index] == '~')
					{
						const std::string::size_type end = text.find('~', index + 1);
						if (end != std::string::npos)
						{
							const std::string tag = text.substr(index, end - index + 1);
							if (IsFormattingTag(tag))
							{
								index = end + 1;
								continue;
							}
						}
					}
					visible += text[index++];
				}
				return visible;
			}

			std::vector<std::string> WrapText(const std::string& text, float maxWidth)
			{
				std::vector<std::string> lines;
				std::string line;
				std::string word;
				std::string wordStartFormatting;
				std::string activeFormatting;

				auto beginWord = [&]()
				{
					if (word.empty()) wordStartFormatting = activeFormatting;
				};
				auto appendLine = [&](bool allowEmpty)
				{
					if (allowEmpty || !GetVisibleText(line).empty()) lines.push_back(line);
					line.clear();
				};
				auto appendWord = [&]()
				{
					if (word.empty()) return;
					const std::string candidate = line.empty() ? wordStartFormatting + word : line + " " + word;
					if (!line.empty() && Print::GetTextWidth(GetVisibleText(candidate)) > maxWidth)
					{
						appendLine(false);
						line = wordStartFormatting + word;
					}
					else
					{
						line = candidate;
					}
					word.clear();
					wordStartFormatting.clear();
				};

				for (std::string::size_type index = 0; index < text.length();)
				{
					if (text[index] == '\r')
					{
						++index;
						continue;
					}
					if (text[index] == '\n')
					{
						appendWord();
						appendLine(true);
						++index;
						continue;
					}
					if (text[index] == '~')
					{
						const std::string::size_type end = text.find('~', index + 1);
						if (end != std::string::npos)
						{
							const std::string tag = text.substr(index, end - index + 1);
							if (tag == "~n~")
							{
								appendWord();
								appendLine(true);
								index = end + 1;
								continue;
							}
							beginWord();
							word += tag;
							if (tag == "~s~") activeFormatting.clear();
							else if (IsPersistentFormattingTag(tag)) activeFormatting += tag;
							index = end + 1;
							continue;
						}
					}
					if (text[index] == ' ' || text[index] == '\t')
					{
						appendWord();
						++index;
						continue;
					}
					beginWord();
					word += text[index++];
				}

				appendWord();
				if (!line.empty() || lines.empty()) appendLine(true);
				return lines;
			}

			std::string ResolveNotificationText(std::string text)
			{
				text = Language::TranslateToSelected(std::move(text));
				if (!DOES_TEXT_LABEL_EXIST(text.c_str())) return text;
				const char* resolved = GET_FILENAME_FOR_AUDIO_CONVERSATION(text.c_str());
				return resolved == nullptr ? text : resolved;
			}

			std::string TruncateNotificationText(std::string text, size_t maximumLength)
			{
				if (text.length() <= maximumLength) return text;
				text.resize(maximumLength);
				if (const auto lastTagStart = text.rfind('~'); lastTagStart != std::string::npos && text.find('~', lastTagStart + 1) == std::string::npos)
					text.resize(lastTagStart);
				return text;
			}

			std::string PrepareNotificationText(std::string text, size_t maximumLength)
			{
				return TruncateNotificationText(ResolveNotificationText(std::move(text)), maximumLength);
			}

			void PrepareNotification(QueuedNotification& notification, DWORD now)
			{
				Print::SetupDraw(font_options, Vector2(descriptionTextScale, descriptionTextScale), false, false, false, optiontext);
				notification.descriptionLines = WrapText(notification.description, textWidth);
				notification.panelHeight = topPadding + bottomPadding +
					(notification.title.has_value() ? titleLineHeight + titleDescriptionGap : 0.0f) +
					(notification.descriptionLines.size() * descriptionLineHeight);

				const size_t visibleCharacterCount = GetVisibleText(notification.title.value_or(std::string()) + notification.description).length();
				const DWORD calculatedReadingDuration = 1500 + static_cast<DWORD>(visibleCharacterCount) * 30;
				const DWORD readingDuration = calculatedReadingDuration > 15000 ? 15000 : calculatedReadingDuration;
				notification.displayDuration = (std::max)(notification.requestedDuration, readingDuration);
				notification.visibleUntil = 0;
				notification.animationStarted = now;
				notification.animation = NotificationAnimation::Entering;
			}

			void StartPendingNotification(DWORD now)
			{
				if (!pendingNotification) return;
				activeNotification = std::move(pendingNotification);
				pendingNotification.reset();
				PrepareNotification(*activeNotification, now);
			}

			int ApplyOpacity(UINT8 alpha, float opacity)
			{
				return static_cast<int>(static_cast<float>(alpha) * opacity);
			}

			bool IsDuplicate(const QueuedNotification& candidate)
			{
				if (activeNotification && activeNotification->title == candidate.title && activeNotification->description == candidate.description)
					return true;

				if (pendingNotification && pendingNotification->title == candidate.title && pendingNotification->description == candidate.description)
					return true;

				return false;
			}

			void EnqueueNotification(QueuedNotification notification)
			{
				if (IsDuplicate(notification)) return;
				pendingNotification = std::move(notification);
				if (activeNotification && activeNotification->animation != NotificationAnimation::Exiting)
				{
					activeNotification->animation = NotificationAnimation::Exiting;
					activeNotification->animationStarted = GetTickCount();
				}
			}
		}

		void ShowNotification(const std::string& title, const std::string& description, float displayTimeInSeconds)
		{
			QueuedNotification notification;
			const std::string resolvedTitle = PrepareNotificationText(title, maximumTitleLength);
			if (!GetVisibleText(resolvedTitle).empty()) notification.title = resolvedTitle;
			notification.description = PrepareNotificationText(description, maximumDescriptionLength);
			if (GetVisibleText(notification.description).empty()) return;
			notification.requestedDuration = displayTimeInSeconds > 0.0f ? static_cast<DWORD>(displayTimeInSeconds * 1000.0f) : 0;
			EnqueueNotification(std::move(notification));
		}

		void ShowNotification(const std::string& description, float displayTimeInSeconds)
		{
			QueuedNotification notification;
			notification.description = PrepareNotificationText(description, maximumDescriptionLength);
			if (GetVisibleText(notification.description).empty()) return;
			notification.requestedDuration = displayTimeInSeconds > 0.0f ? static_cast<DWORD>(displayTimeInSeconds * 1000.0f) : 0;
			EnqueueNotification(std::move(notification));
		}

		void ShowNotification(std::ostream& description, float displayTimeInSeconds)
		{
			ShowNotification(dynamic_cast<std::ostringstream&>(description).str(), displayTimeInSeconds);
		}

		void ShowNotification(std::wostream& description, float displayTimeInSeconds)
		{
			const std::wstring text = dynamic_cast<std::wostringstream&>(description).str();
			ShowNotification(std::string(text.begin(), text.end()), displayTimeInSeconds);
		}

		void AdvanceNotificationState(DWORD now)
		{
			if (!activeNotification)
			{
				StartPendingNotification(now);
			}
			if (!activeNotification) return;

			if (activeNotification->animation == NotificationAnimation::Entering && now - activeNotification->animationStarted >= animationDuration)
			{
				activeNotification->animation = NotificationAnimation::Visible;
				activeNotification->animationStarted = now;
				activeNotification->visibleUntil = now + activeNotification->displayDuration;
			}
			else if (activeNotification->animation == NotificationAnimation::Visible && now >= activeNotification->visibleUntil)
			{
				activeNotification->animation = NotificationAnimation::Exiting;
				activeNotification->animationStarted = now;
			}
			else if (activeNotification->animation == NotificationAnimation::Exiting && now - activeNotification->animationStarted >= animationDuration)
			{
				activeNotification.reset();
				StartPendingNotification(now);
			}
		}

		void RenderNotification(DWORD now)
		{
			QueuedNotification& notification = *activeNotification;
			float progress = 1.0f;
			if (notification.animation == NotificationAnimation::Entering)
				progress = (std::min)(1.0f, static_cast<float>(now - notification.animationStarted) / animationDuration);
			else if (notification.animation == NotificationAnimation::Exiting)
				progress = 1.0f - (std::min)(1.0f, static_cast<float>(now - notification.animationStarted) / animationDuration);
			notification.panelHeight = topPadding + bottomPadding +
				(notification.title.has_value() ? titleLineHeight + titleDescriptionGap : 0.0f) +
				(notification.descriptionLines.size() * descriptionLineHeight);
			const float panelX = (1.0f - panelWidth) / 2.0f;
			float verticalOffset = 0.0f;
			if (notification.animation == NotificationAnimation::Entering)
				verticalOffset = (1.0f - progress) * animationTravelDistance;
			else if (notification.animation == NotificationAnimation::Exiting)
				verticalOffset = -(1.0f - progress) * animationTravelDistance;
			const float panelY = 1.0f - notificationBottomMargin - notification.panelHeight + verticalOffset;
			const float textX = panelX + stripeWidth + leftPadding;

			DRAW_RECT(panelX + panelWidth / 2.0f, panelY + notification.panelHeight / 2.0f, panelWidth, notification.panelHeight, BG.R, BG.G, BG.B, ApplyOpacity(BG.A, progress), false);
			DRAW_RECT(panelX + stripeWidth / 2.0f, panelY + notification.panelHeight / 2.0f, stripeWidth, notification.panelHeight, titlebox.R, titlebox.G, titlebox.B, ApplyOpacity(titlebox.A, progress), false);

			float descriptionY = panelY + topPadding;
			if (notification.title.has_value())
			{
				Print::SetupDraw(font_title, Vector2(titleTextScale, titleTextScale), false, false, true, { titletext.R, titletext.G, titletext.B, static_cast<UINT8>(ApplyOpacity(titletext.A, progress)) });
				Print::drawstring(*notification.title, textX, descriptionY);
				descriptionY += titleLineHeight + titleDescriptionGap;
			}

			for (const auto& line : notification.descriptionLines)
			{
				Print::SetupDraw(font_options, Vector2(descriptionTextScale, descriptionTextScale), false, false, false, { optiontext.R, optiontext.G, optiontext.B, static_cast<UINT8>(ApplyOpacity(optiontext.A, progress)) }, { 0.0f, textX + textWidth });
				Print::drawstring(line, textX, descriptionY);
				descriptionY += descriptionLineHeight;
			}
		}

		void TickNotifications()
		{
			const DWORD now = GetTickCount();
			AdvanceNotificationState(now);
			if (activeNotification) RenderNotification(now);
		}

		// Messages - Errors
		void PrintErrorInvalidInput(std::string inputStr)
		{
			Game::Print::ShowNotification("~r~Error:", "Invalid Input: " + inputStr);
			addlog(ige::LogType::LOG_ERROR, "Invalid Input: " + inputStr);
		}
		void PrintErrorInvalidModel(std::string inputStr)
		{
			Game::Print::ShowNotification("~r~Error:", "Invalid Model: " + inputStr);
			addlog(ige::LogType::LOG_ERROR, "Invalid Model: " + inputStr);
		}

		// Text width
		float GetTextWidth(const std::string& s, bool gxt)
		{
			if (gxt)
				BEGIN_TEXT_COMMAND_GET_SCREEN_WIDTH_OF_DISPLAY_TEXT(s.c_str());
			else
			{
				if (s.length() < 100)
				{
					BEGIN_TEXT_COMMAND_GET_SCREEN_WIDTH_OF_DISPLAY_TEXT("STRING");
					ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME(s.c_str());
				}
				else
				{
					BEGIN_TEXT_COMMAND_GET_SCREEN_WIDTH_OF_DISPLAY_TEXT("jamyfafi");
					add_text_component_long_string(s);
				}
			}
			return END_TEXT_COMMAND_GET_SCREEN_WIDTH_OF_DISPLAY_TEXT(1);
		}
		float GetTextWidth(int inumber)
		{
			BEGIN_TEXT_COMMAND_GET_SCREEN_WIDTH_OF_DISPLAY_TEXT("NUMBER");
			ADD_TEXT_COMPONENT_INTEGER(inumber);
			return END_TEXT_COMMAND_GET_SCREEN_WIDTH_OF_DISPLAY_TEXT(1);
		}
		float GetTextWidth(float fnumber, UINT8 decimal_places)
		{
			BEGIN_TEXT_COMMAND_GET_SCREEN_WIDTH_OF_DISPLAY_TEXT("NUMBER");
			ADD_TEXT_COMPONENT_FLOAT(fnumber, decimal_places);
			return END_TEXT_COMMAND_GET_SCREEN_WIDTH_OF_DISPLAY_TEXT(1);
		}
	}

	//On screen keyboard
	std::string InputBox(const std::string& escReturn, int maxChars, std::string titlegxt, std::string preText)
	{
		preText = preText.substr(0, maxChars);

		//CustomKeyboardText ckt;
		DISPLAY_ONSCREEN_KEYBOARD(true, "", "", preText.c_str(), "", "", "", maxChars);

		bool pasteWasPressed = false;

		while (UPDATE_ONSCREEN_KEYBOARD() == 0)
		{
			bool ctrlDown = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
			bool vDown = (GetAsyncKeyState('V') & 0x8000) != 0;

			bool shiftDown = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
			bool insertDown = (GetAsyncKeyState(VK_INSERT) & 0x8000) != 0;

			bool ctrlV = ctrlDown && vDown;
			bool shiftInsert = shiftDown && insertDown;

			if ((ctrlV || shiftInsert) && !pasteWasPressed)
			{
				pasteWasPressed = true;

				std::string clip = GetClipboardText();
				if (!clip.empty())
				{
					clip = clip.substr(0, maxChars);

					CANCEL_ONSCREEN_KEYBOARD();

					WAIT(0);

					DISPLAY_ONSCREEN_KEYBOARD(true, "", "", clip.c_str(), "", "", "", maxChars);
				}
			}

			if (!ctrlV && !shiftInsert) pasteWasPressed = false;

			SET_TEXT_FONT(/*GTAfont::Arial*/0);
			SET_TEXT_SCALE(0.34f, 0.34f);
			SET_TEXT_COLOUR(255, 255, 255, 255);
			SET_TEXT_WRAP(0.0f, 1.0f);
			SET_TEXT_RIGHT_JUSTIFY(FALSE);
			SET_TEXT_CENTRE(TRUE);
			SET_TEXT_DROPSHADOW(0, 0, 0, 0, 0);
			SET_TEXT_EDGE(0, 0, 0, 0, 0);
			SET_TEXT_OUTLINE();

			if (DOES_TEXT_LABEL_EXIST(titlegxt.c_str()))
			{
				BEGIN_TEXT_COMMAND_DISPLAY_TEXT(titlegxt.c_str());
			}
			else
			{
				titlegxt = Language::TranslateToSelected(titlegxt);
				if (titlegxt.length() < 100)
				{
					BEGIN_TEXT_COMMAND_DISPLAY_TEXT("STRING");
					ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME(titlegxt.c_str());
				}
				else
				{
					BEGIN_TEXT_COMMAND_DISPLAY_TEXT("jamyfafi");
					add_text_component_long_string(titlegxt);
				}
			}
			END_TEXT_COMMAND_DISPLAY_TEXT(0.5f, 0.37f, 0);
			WAIT(0);
		}
		if (UPDATE_ONSCREEN_KEYBOARD() == 2)
		{
			return escReturn;
		}

		return GET_ONSCREEN_KEYBOARD_RESULT();
	}

	//PLAYER_PED_ID()
	GTAplayer Player()
	{
		return PLAYER::PLAYER_ID();
	}
	GTAplayer Player(int index)
	{
		return GTAplayer(index);
	}
	GTAped PlayerPed()
	{
		return PLAYER::PLAYER_PED_ID();
	}
	GTAped PlayerPed(int index)
	{
		return PLAYER::GET_PLAYER_PED(index);
	}
}
