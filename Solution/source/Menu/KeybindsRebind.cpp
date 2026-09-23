#include "Keybinds.h"

#include "..\macros.h"

#include "Menu.h"
#include "submenu_enum.h"

#include "..\Natives\natives2.h"
#include "..\Scripting\enums.h"
#include "..\Scripting\Game.h"
#include "..\Util\keyboard.h"
#include "..\Util\FileLogger.h"

#include <string>

namespace Keybinds
{
	constexpr UINT16 MaxControlId = 351;

	// Capture flow: AwaitPress -> (PrimaryHeld | ConfirmOverwrite) -> commit or cancel.
	enum class RebindStage : UINT8 { Idle, AwaitPress, PrimaryHeld, ConfirmOverwrite };

	namespace
	{
		RebindStage g_stage = RebindStage::Idle;
		const KeybindEntry* g_entry = nullptr;
		Context g_context = Context::Auto;   // resolved input device (never Auto while capturing)
		UINT16 g_primary = NoBind;           // captured primary key/button awaiting release or a combo partner
		UINT16 g_confirmValue = NoBind;      // key/button awaiting overwrite confirmation
		DWORD g_lastNotify = 0;

		bool ControllerPressed(UINT16 control)
		{
			return IS_DISABLED_CONTROL_PRESSED(ControlIndex(control), control);
		}

		bool ControllerJustReleased(UINT16 control)
		{
			return IS_DISABLED_CONTROL_JUST_RELEASED(ControlIndex(control), control);
		}

		// check if any of the physical-A aliases are pressed, to avoid rebinding them when the user is pressing them to confirm a menu action.
		bool PhysicalAPressed()
		{
			if (!ControllerPressed(INPUT_FRONTEND_ACCEPT)) return false;
			for (UINT16 id : PhysicalAControls())
				if (ControllerPressed(id))
					return true;
			return false;
		}

		// Gamepad: the ACCEPT/SELECT buttons that opened the menu must be unpressed before
		// scanning, so holding them doesn't rebind immediately.
		bool ButtonsIdle()
		{
			return !ControllerPressed(INPUT_FRONTEND_ACCEPT) && !ControllerPressed(INPUT_CELLPHONE_SELECT);
		}


		// Sweeps the gamepad for a candidate button press, ignoring the menu's opening buttons and physical-A aliases. Returns NoBind if none found.
		UINT16 SweepGamepadCandidate()
		{
			for (UINT16 c = 0; c <= MaxControlId; ++c)
			{
				// ACCEPT/SELECT/RRIGHT/LEFT are menu keys, not bindable.
				if (c == INPUT_FRONTEND_ACCEPT || c == INPUT_CELLPHONE_SELECT || c == INPUT_FRONTEND_RRIGHT
					|| c == INPUT_FRONTEND_LEFT || (IsPhysicalA(c) && PhysicalAPressed())) continue;
				if (IS_DISABLED_CONTROL_JUST_PRESSED(ControlIndex(c), c))
					return c;
			}
			return NoBind;
		}

		// True when another entry in the same category already has the same keybind.
		bool FindConflict(UINT16 value, const KeybindEntry* captureEntry)
		{
			const bool keyboard = g_context == Context::Keyboard;
			for (const auto& entry : Registry())
			{
				if (&entry == captureEntry) continue;
				if (entry.Category() != captureEntry->Category()) continue; // cross-category duplicates are intentional
				if (keyboard && (entry.KeyboardKey1() == value || entry.KeyboardKey2() == value)) return true;
				if (!keyboard && (entry.GamepadButton1() == value || entry.GamepadButton2() == value)) return true;
			}
			return false;
		}

		// Returns "key" or "button" for prompt wording.
		std::string SideName()
		{
			return g_context == Context::Keyboard ? "key" : "button";
		}

		std::string ContextName()
		{
			return g_context == Context::Keyboard ? "Keyboard" : "Gamepad";
		}

		std::string SideGlyph(UINT16 value)
		{
			return g_context == Context::Keyboard ? KeyboardKeyName(value) : GamepadButtonName(value);
		}

		// Yellow-highlighted key/button name so it stands out in prompts and notifications.
		std::string ColoredGlyph(UINT16 value)
		{
			return "~y~" + SideGlyph(value) + "~s~";
		}

		// Per-context capture-cancel gestures shown as a dim trailing hint line.
		std::string CancelHintText()
		{
			return g_context == Context::Keyboard
				? "~c~ESC / Numpad0 cancel, Backspace unbinds~s~"
				: "~c~B cancels, D-Pad Left unbinds~s~";
		}

		std::string WaitingPromptText()
		{
			return "Rebinding ~b~" + g_entry->Label() + "~s~\n~b~Waiting for input...~s~\n"
				+ CancelHintText();
		}

		std::string HeldPromptText()
		{
			const std::string side = g_context == Context::Keyboard ? "Key" : "Button";
			return side + " " + ColoredGlyph(g_primary)
				+ " held. ~g~Release to apply~s~, or press another " + SideName()
				+ " for a combo.\n" + CancelHintText();
		}

		std::string OverwritePromptText()
		{
			return ColoredGlyph(g_confirmValue) + " is ~r~already bound~s~.\nPress the same "
				+ SideName() + " again to overwrite, or a different one to cancel.\n"
				+ CancelHintText();
		}

		void DrawRebindPrompt(const std::string& text)
		{
			Game::Print::SetupDraw(GTAfont::Arial, Vector2(0.35f, 0.35f), true, false, true);
			Game::Print::DrawString(oss_ << text, 0.5f, 0.72f);
		}

		void PromptRebindState()
		{
			switch (g_stage)
			{
				case RebindStage::AwaitPress: DrawRebindPrompt(WaitingPromptText()); break;
				case RebindStage::PrimaryHeld: DrawRebindPrompt(HeldPromptText()); break;
				case RebindStage::ConfirmOverwrite: DrawRebindPrompt(OverwritePromptText()); break;
				default: break;
			}
		}

		void NotifyConflict(UINT16 conflictValue)
		{
			if (g_lastNotify && GetTickCount() - g_lastNotify < 250) return;
			g_lastNotify = GetTickCount();
			Game::Print::ShowNotification("~r~Keybind conflict~s~",
				ColoredGlyph(conflictValue) + " is already in use.\n~c~Press the same " + SideName()
				+ " again to overwrite,\nor a different one to cancel.~s~", 2.0f);
		}

		// Writes the captured value(s) on the capture side; combos write both members.
		void SetBinds(UINT16 first, UINT16 second = NoBind)
		{
			if (g_context == Context::Keyboard)
			{
				g_entry->SetKeyboardKey1(first);
				g_entry->SetKeyboardKey2(second);
			}
			else
			{
				g_entry->SetGamepadButton1(first);
				g_entry->SetGamepadButton2(second);
			}
		}

		// Commits the capture, reports it, and ends the capture session.
		void Commit(UINT16 first, UINT16 second = NoBind)
		{
			const bool combo = second != NoBind;
			const std::string glyph = SideGlyph(first) + (combo ? " + " + SideGlyph(second) : "");

			SetBinds(first, second);
			Save();
			addlog(ige::LogType::LOG_DEBUG,
				"Keybinds: committed \"" + g_entry->Id() + "\" = " + glyph
				+ (combo ? " (combo)" : "") + " (" + ContextName() + ")");
			Game::Print::ShowNotification("~b~" + g_entry->Label() + "~s~",
				"Bound to ~g~" + glyph + "~s~", 2.0f);
			CancelRebind();
		}

		void Unbind()
		{
			SetBinds(NoBind, NoBind);
			Save();
			addlog(ige::LogType::LOG_DEBUG, "Keybinds: \"" + g_entry->Id() + "\" unbound");
			Game::Print::ShowNotification("~b~" + g_entry->Label() + "~s~", "~o~unbound~s~", 2.0f);
			CancelRebind();
		}

		// Return (Enter) and Numpad5 are the accept keys that open the capture; Escape/Numpad0
		// cancel it, Backspace unbinds. Excluding them keeps menu navigation and the unbind
		// gesture unhijackable.
		bool IsReservedKey(DWORD vk)
		{
			return vk == VirtualKey::Escape || vk == VirtualKey::Numpad0
				|| vk == VirtualKey::Return || vk == VirtualKey::Numpad5
				|| vk == VirtualKey::Back;
		}

		// AltGr layouts (e.g. Polish Programmers) synthesize a right-Alt press as a Control key
		// with the Alt modifier held; GetAsyncKeyState reports it as right Alt elsewhere, so
		// store it as Right Menu. A real Control+Alt chord also maps here - rare and harmless.
		UINT16 NormalizeCandidate(DWORD vk)
		{
			if (vk == VirtualKey::Control && (get_key_pressed(VirtualKey::Menu) || get_key_pressed(VirtualKey::LeftMenu) || get_key_pressed(VirtualKey::RightMenu)))
				return VirtualKey::RightMenu;
			return (UINT16)vk;
		}

		// Sweeps the keyboard for a candidate keypress, ignoring reserved keys and keys already held down. Returns NoBind if none found.
		UINT16 SweepKeyboardCandidate()
		{
			for (DWORD k = 8; k <= 0xFE; ++k)
			{
				if (IsReservedKey(k)) continue;
				if (IsKeyDown(k) || IsKeyJustUp(k, false))
					return NormalizeCandidate(k);
			}
			return NoBind;
		}

		bool RebindTickKeyboard()
		{
			// ESC / Numpad0 cancel the capture from any stage.
			if (IsKeyJustUp(VirtualKey::Escape, false) || IsKeyJustUp(VirtualKey::Numpad0, false))
			{
				CancelRebind();
				return true;
			}

			switch (g_stage)
			{
				// Awaiting a keypress to capture. The first key pressed becomes the primary; if it's already bound, the user must confirm overwrite.
				case RebindStage::AwaitPress:
				{
					if (IsKeyJustUp(VirtualKey::Back, false))
					{
						Unbind();
						return true;
					}

					UINT16 candidate = SweepKeyboardCandidate();
					if (candidate == NoBind)
					{
						DrawRebindPrompt(WaitingPromptText());
						return false;
					}

					if (FindConflict(candidate, g_entry))
					{
						g_stage = RebindStage::ConfirmOverwrite;
						g_confirmValue = candidate;
						NotifyConflict(candidate);
					}
					else
					{
						g_primary = candidate;
						g_stage = RebindStage::PrimaryHeld;
					}
					return true;
				}

				// The primary key is held; the user can release it to commit, or press a second key to form a combo. Backspace unbinds.
				case RebindStage::PrimaryHeld:
				{	
					if (IsKeyJustUp(VirtualKey::Back, false))
					{
						Unbind();
						return true;
					}
					if (g_primary == NoBind)
					{
						addlog(ige::LogType::LOG_WARNING, "Rebind capture: PrimaryHeld stage with no key/button held - capture cancelled");
						CancelRebind();
						return true;
					}
					
					if (get_key_pressed(g_primary))
					{
						// The primary key is still held; check for a second key to form a combo. Ignore the primary and reserved keys.
						for (DWORD k = 8; k <= 0xFE; ++k)
						{	
							if (k == (DWORD)g_primary || IsReservedKey(k)) continue;
							if (IsKeyJustUp(k, false))
							{
								ResetKeyState(k);
								Commit(g_primary, NormalizeCandidate(k));
								return true;
							}
						}
					}
					// If the primary key is released, commit it. The user can also press a different key to cancel the capture.
					else if (IsKeyJustUp(g_primary, false))
					{
						Commit(g_primary);
						return true;
					}

					DrawRebindPrompt(HeldPromptText());
					return false;
				}

				// The primary key was already bound; the user must press it again to confirm overwrite, or a different key to cancel.
				case RebindStage::ConfirmOverwrite:
				{
					UINT16 candidate = SweepKeyboardCandidate();
					if (candidate == NoBind)
					{
						DrawRebindPrompt(OverwritePromptText());
						return false;
					}
					if (candidate == g_confirmValue)
					{
						g_primary = g_confirmValue;
						g_stage = RebindStage::PrimaryHeld;
					}
					else
					{
						CancelRebind(); // a different key pressed: abort the overwrite
					}
					return true;
				}

				default: return false;
			}
		}

		bool RebindTickGamepad()
		{
			// Menu back button (Xbox B) cancels the capture from any stage.
			if (IS_DISABLED_CONTROL_JUST_PRESSED(ControlIndex(INPUT_FRONTEND_RRIGHT), INPUT_FRONTEND_RRIGHT))
			{
				CancelRebind();
				return true;
			}

			switch (g_stage)
			{
				case RebindStage::AwaitPress:
				{
					// DPad Left unbinds the entry.
					if (IS_DISABLED_CONTROL_JUST_PRESSED(ControlIndex(INPUT_FRONTEND_LEFT), INPUT_FRONTEND_LEFT))
					{
						Unbind();
						return true;
					}

					if (!ButtonsIdle() || PhysicalAPressed())
					{
						DrawRebindPrompt(WaitingPromptText());
						return false;
					}

					UINT16 candidate = SweepGamepadCandidate();
					if (candidate == NoBind)
					{
						DrawRebindPrompt(WaitingPromptText());
						return false;
					}

					if (FindConflict(candidate, g_entry))
					{
						g_stage = RebindStage::ConfirmOverwrite;
						g_confirmValue = candidate;
						NotifyConflict(candidate);
					}
					else
					{
						g_primary = candidate;
						g_stage = RebindStage::PrimaryHeld;
					}
					return true;
				}

				case RebindStage::PrimaryHeld:
				{
					// The primary button is held; the user can release it to commit, or press a second button to form a combo. DPad Left unbinds.
					UINT16 second = SweepGamepadCandidate();
					if (second != NoBind && second != g_primary)
					{
						Commit(g_primary, second);
						return true;
					}
					if (ControllerJustReleased(g_primary))
					{
						Commit(g_primary);
						return true;
					}
					PromptRebindState();
					return false;
				}

				case RebindStage::ConfirmOverwrite:
				{
					if (IS_DISABLED_CONTROL_JUST_PRESSED(ControlIndex(g_confirmValue), g_confirmValue))
					{
						g_primary = g_confirmValue;
						g_stage = RebindStage::PrimaryHeld;
						return true;
					}
					if (SweepGamepadCandidate() != NoBind)
					{
						CancelRebind(); // a different button pressed: abort the overwrite
						return true;
					}
					PromptRebindState();
					return false;
				}

				default: return false;
			}
		}
	}

	bool IsRebinding() { return g_stage != RebindStage::Idle; }

	bool IsRebinding(const KeybindEntry* entry, Context context)
	{
		if (!IsRebinding() || g_entry != entry) return false;
		return g_context == ResolveContext(context);
	}

	void StartRebind(const KeybindEntry& entry, Context context)
	{
		g_context = ResolveContext(context);
		g_entry = &entry;
		g_primary = NoBind;
		g_confirmValue = NoBind;
		g_stage = RebindStage::AwaitPress;

		if (g_context == Context::Keyboard)
		{
			// The Enter (accept) press that opened the capture is still latched in ScriptHook's
			// keyStates; flush all keys so the first sweep can't immediately rebind that press.
			for (DWORD k = 8; k <= 0xFE; ++k)
				ResetKeyState(k);
		}
	}

	void CancelRebind()
	{
		g_stage = RebindStage::Idle;
		g_entry = nullptr;
		g_primary = NoBind;
		g_confirmValue = NoBind;
	}

	bool RebindTick()
	{
		if (!IsRebinding()) return false;
		if (g_entry == nullptr)
		{
			// Without the cancel the capture state would swallow all menu input forever.
			addlog(ige::LogType::LOG_ERROR, "RebindTick: capture active with null entry - capture cancelled");
			CancelRebind();
			return false;
		}
		if (g_context == Context::Keyboard)
			return RebindTickKeyboard();
		else if (g_context == Context::Gamepad)
			return RebindTickGamepad();
		return false;
	}
}
