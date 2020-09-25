#ifndef ACE_GUI_H_
#define ACE_GUI_H_

#include "player_entity.h"
#include "extended_player_entity.h"

#include <cstdint>
#include <string>

namespace gui {
	HWND InitGui();
	void Render();
	void StartFrame();
	void Cleanup(HWND window_handle);
	void ShowPosition();
	void ShowLocalPlayerInformation(const PlayerEntity player);
	void ShowPlayerInformation(const std::vector<ExtendedPlayerEntity> players, const float fov);
	void ShowExtraPlayerInformation();
	void ShowOptions(float* fov);
}

#endif // ACE_GUI_H_
