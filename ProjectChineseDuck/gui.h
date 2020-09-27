#ifndef ACE_GUI_H_
#define ACE_GUI_H_

#include "extended_player_entity.h"
#include "settings.h"

namespace gui {
	HWND InitGui();
	void Render();
	void StartFrame();
	void Cleanup(HWND window_handle);
	void ShowLocalPlayerInformation(const PlayerEntity player);
	void ShowPlayerInformation(const std::vector<ExtendedPlayerEntity> players, const float fov);
	void ShowExtraPlayerInformation();
	void ShowOptions(Settings* settings);
}

#endif // ACE_GUI_H_
