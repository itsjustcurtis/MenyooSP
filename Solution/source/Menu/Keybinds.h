#pragma once

#include <string>
#include <vector>

typedef unsigned short UINT16;
typedef unsigned char UINT8;

namespace Keybinds
{
	constexpr UINT16 NoBind = 0xFFFF;

	constexpr const char* UnboundGlyph = "\xe2\x80\x94"; // "—"

	// control index 0 = gameplay, 2 = frontend; the natives reject the wrong index for a control.
	inline int ControlIndex(UINT16 control) { return control < 50 ? 0 : 2; }

	enum class Context : UINT8 { Auto, Keyboard, Gamepad };

	// Context::Auto -> detect whether user is using keyboard or controller input and return the appropriate context. Otherwise returns the given context.
	Context ResolveContext(Context context);

	enum class GamepadLabelStyle : UINT8 { Xbox, PlayStation };
	extern GamepadLabelStyle gamepadLabelStyle;

	class KeybindEntry
	{
	public:
		KeybindEntry(const std::string& id, const std::string& label, const std::string& description,
			UINT16 keyboardKey1, UINT16 keyboardKey2, UINT16 gamepadButton1, UINT16 gamepadButton2,
			const std::string& category, bool useDisabledControls = true);

		const std::string& Id() const;
		const std::string& Label() const;
		const std::string& Description() const;
		const std::string& Category() const;

		UINT16 KeyboardKey1() const { return m_keyboardKey1; }
		UINT16 KeyboardKey2() const { return m_keyboardKey2; }
		UINT16 GamepadButton1() const { return m_gamepadButton1; }
		UINT16 GamepadButton2() const { return m_gamepadButton2; }

		void SetKeyboardKey1(UINT16 value) const { m_keyboardKey1 = value; }
		void SetKeyboardKey2(UINT16 value) const { m_keyboardKey2 = value; }
		void SetGamepadButton1(UINT16 value) const { m_gamepadButton1 = value; }
		void SetGamepadButton2(UINT16 value) const { m_gamepadButton2 = value; }

		// Was the bind used this frame? 
		// Combo = member 1 held + member 2 tapped (see Menu::isBinds).
		bool WasPressedThisFrame(Context context) const;
		// Was the bind let go this frame? Hold-style actions unfreeze/unmute on this edge.
		// Combo = member 1 held + member 2 released (see Menu::isBinds).
		bool WasReleasedThisFrame(Context context) const;
		// Is it being held? Single = primary held; combo = both members held.
		bool IsHeld(Context context) const;
		// Right-side row text / free-text glyph: "K + L", "RB + LS", "SPRINT" or "—".
		std::string Glyph(Context context) const;

	private:
		std::string m_id, m_label, m_description, m_category;
		mutable UINT16 m_keyboardKey1, m_keyboardKey2;
		mutable UINT16 m_gamepadButton1, m_gamepadButton2;
		bool m_useDisabledControls = true;  // use IS_DISABLED_CONTROL_* natives (work over the menu) for gamepad buttons when true
	};

	const std::vector<KeybindEntry>& Registry();
	const KeybindEntry* FindEntry(const std::string& id);
	// dynamic list of all categories in the registry, for the category headers in the keybinds menu.
	const std::vector<std::string>& Categories();

	// id-based convenience API - access the registry by id instead of by KeybindEntry reference. Returns false if the id is unknown.
	bool WasPressedThisFrame(const std::string& id, Context context = Context::Auto);
	bool WasReleasedThisFrame(const std::string& id, Context context = Context::Auto);
	bool IsHeld(const std::string& id, Context context = Context::Auto);
	std::string GetGlyph(const std::string& id, Context context = Context::Auto);

	// row text helpers
	std::string KeyboardKeyName(UINT16 vk);
	// Full instructional-buttons icon id for a keyboard key ("t_K", "T_CTRL", "w_BACKSPACE").
	// Consumed only by the instructional_buttons scaleform, never inside display text.
	std::string KeyboardKeyIbId(UINT16 vk);
	std::string GamepadButtonName(UINT16 control);
	// Aliases for all physical "A" buttons (Xbox "A", PS "Cross")
	const std::vector<UINT16>& PhysicalAControls();
	bool IsPhysicalA(UINT16 control);
	// Draws the entry's current input glyph as an add_IB prompt button. It draws keyboard glyphs when the menu is using keyboard input, and gamepad glyphs when using controller input.
	void AddBindIB(const std::string& id, const std::string& text);
	// Draws the entry's current input as an add_IB prompt button (uses onText when active == true, offText when not).
	void AddBindIB(const std::string& id, bool active, const std::string& onText, const std::string& offText);
	// True when this entry's keys/buttons would actively fight another bind in the same category (capture excluded).
	bool HasConflict(const KeybindEntry& entry, Context context);
	// entry.Glyph(context) wrapped "~r~…~s~" on same-category conflict, grey "~c~—~s~" when unbound. Used only for the right-side row text in the keybinds menu.
	std::string ColorizeRowText(const KeybindEntry& entry, Context context);

	// Rebinding API 
	bool IsRebinding();
	bool IsRebinding(const KeybindEntry* entry, Context context);
	void StartRebind(const KeybindEntry& entry, Context context);
	void CancelRebind();
	bool RebindTick(); // consumed a press/release this frame
	bool IsKeybindSubmenu();

	// Persistence (called from MenuConfig::ConfigRead). Reads menyooStuff\Keybinds.xml,
	// converting the legacy menyooConfig.ini bind keys on first run.
	void Load();
	void Save();
}