#pragma once

namespace sub::Spooner::ImGuiSpooner
{
	bool Initialize();
	void Shutdown();

	void Tick();

	void SetVisible(bool visible);
	bool IsVisible();
}
