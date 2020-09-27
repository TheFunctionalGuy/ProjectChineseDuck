#include <algorithm>
#include <array>
#include <iostream>

#include "extended_player_entity.h"
#include "gui.h"
#include "memory.h"
#include "process.h"
#include "settings.h"
#include "vector.h"

// Forward declaration
void CalculateYawAndPitchToOtherPlayer(const std::array<float, 3>& local_player_position, const std::array<float, 3>& target_position, float& yaw, float& pitch);
std::array<float, 3> CorrectHeadPosition(const PlayerEntity& player);

// Pi constant
constexpr float kPi = 3.141592653589793238463f;

int main() {
	HWND window_handle = gui::InitGui();

	// Init GUI
	if (window_handle == NULL) {
		std::cout << "There was an error initializing the GUI." << std::endl;

		return 1;
	}

	// Settings
	Settings settings;
	settings.fov = 30.0f;
	settings.aim_for_head = false;
	settings.ignore_teams = true;
	settings.no_recoil = false;
	settings.unlimited_ammo = false;
	settings.unlimited_health = false;
	settings.unlimited_armor = false;

	// Base addresses
	uintptr_t module_base_address = NULL;
	uintptr_t player_count_address = NULL;
	uintptr_t local_player_base = NULL;
	uintptr_t entity_list_base = NULL;

	DWORD process_id = GetProcessId(L"ac_client.exe");

	if (process_id) {
		module_base_address = GetModuleBaseAddress(process_id, L"ac_client.exe");

		HANDLE process = 0;
		process = OpenProcess(PROCESS_ALL_ACCESS, NULL, process_id);

		player_count_address = module_base_address + 0x10F500;

		local_player_base = module_base_address + 0x10F4F4;
		entity_list_base = module_base_address + 0x10F4F8;

		// Entities
		unsigned int player_count = 0;
		PlayerEntity local_player;
		std::vector<ExtendedPlayerEntity> players = {};

		// Switches
		bool aimbot = false;

		MSG message;
		ZeroMemory(&message, sizeof(message));

		// Main loop
		while (message.message != WM_QUIT) {
			if (PeekMessage(&message, NULL, 0U, 0U, PM_REMOVE)) {
				TranslateMessage(&message);
				DispatchMessage(&message);

				continue;
			}

			// Get player count
			ReadProcessMemory(process, (BYTE*)player_count_address, &player_count, sizeof(player_count), nullptr);

			// Get local player
			uintptr_t local_player_address = FindDMAAddress(process, local_player_base, { 0x0 });
			ReadProcessMemory(process, (BYTE*)local_player_address, &local_player, sizeof(local_player), nullptr);

			// Get players
			for (unsigned int i = 1; i < player_count; i++) {
				uintptr_t player_address = FindDMAAddress(process, entity_list_base, { i * 0x4, 0x0 });
				PlayerEntity player;

				ReadProcessMemory(process, (BYTE*)player_address, &player, sizeof(player), nullptr);

				player.position_head = CorrectHeadPosition(player);

				float distance = vec::Dist(local_player.position, player.position);
				float yaw;
				float pitch;

				if (settings.aim_for_head) {
					CalculateYawAndPitchToOtherPlayer(local_player.position_head, player.position_head, yaw, pitch);
				} else {
					// Correct body position
					std::array<float, 3> corrected_position = player.position;
					corrected_position[2] += 3.5f;

					CalculateYawAndPitchToOtherPlayer(local_player.position_head, corrected_position, yaw, pitch);
				}

				float y_diff = std::abs(yaw - local_player.angles[0]);
				if (y_diff > 180.0f) y_diff = std::abs(y_diff - 360.0f);
				float z_diff = std::abs(pitch - local_player.angles[1]);
				if (z_diff > 180.0f) z_diff = std::abs(z_diff - 360.0f);

				ExtendedPlayerEntity extended_player = ExtendedPlayerEntity(player, distance, { yaw, pitch }, { y_diff, z_diff });

				players.push_back(extended_player);
			};

			// Sort array by distance to local player
			std::vector<ExtendedPlayerEntity> enemy_players_alive;

			std::sort(players.begin(), players.end(), [](ExtendedPlayerEntity ent1, ExtendedPlayerEntity ent2) {return ent1.distance_to_local_player < ent2.distance_to_local_player; });
			std::copy_if(players.begin(), players.end(), std::back_inserter(enemy_players_alive), [&](ExtendedPlayerEntity ent) {return !ent.player.is_dead && (settings.ignore_teams || local_player.team != ent.player.team); });

			if (GetAsyncKeyState(VK_END) & 1) {
				std::cout << "Program exited." << std::endl;

				return 0;
			}

			// Aimbot active
			if (GetAsyncKeyState(VK_LCONTROL)) {
				for (const auto& ent : enemy_players_alive) {
					if (ent.fov_diff[0] < settings.fov / 2.0f && ent.fov_diff[1] < settings.fov / 2.0f) {
						uintptr_t angles_address = FindDMAAddress(process, local_player_base, { 0x40 });

						mem::PatchBytes((BYTE*)angles_address, (BYTE*)&ent.angles_from_local_player[0], sizeof(ent.angles_from_local_player[0]), process);
						mem::PatchBytes((BYTE*)angles_address + 4, (BYTE*)&ent.angles_from_local_player[1], sizeof(ent.angles_from_local_player[1]), process);

						break;
					}
				}
			}

			// TODO: only patch once
			if (settings.no_recoil) {
				const BYTE patched_bytes[14] = { 0xB8, 0x00, 0x00, 0x00, 0x00, 0x90, 0x90, 0xB9, 0x00, 0x00, 0x00, 0x00, 0x90, 0x90 };
				mem::PatchBytes((BYTE*)module_base_address + 0x6226D, (BYTE*)&patched_bytes, sizeof(patched_bytes), process);
			} else { 
				const BYTE unpatched_bytes[14] = { 0x0F, 0xBF, 0x87, 0x20, 0x01, 0x00, 0x00, 0x0F, 0xBF, 0x8F, 0x22, 0x01, 0x00, 0x00 };
				mem::PatchBytes((BYTE*)module_base_address + 0x6226D, (BYTE*)&unpatched_bytes, sizeof(unpatched_bytes), process);
			}

			// TODO: only patch once
			if (settings.unlimited_ammo) {
				const BYTE patched_bytes[2] = { 0x90, 0x90 };
				mem::PatchBytes((BYTE*)module_base_address + 0x637E9, (BYTE*)&patched_bytes, sizeof(patched_bytes), process);
			} else {
				const BYTE unpatched_bytes[2] = { 0xFF, 0x0E };
				mem::PatchBytes((BYTE*)module_base_address + 0x637E9, (BYTE*)&unpatched_bytes, sizeof(unpatched_bytes), process);
			}

			if (settings.unlimited_health) {
				const int32_t patched_health_value = 1337;
				mem::PatchBytes((BYTE*)local_player_address + 0xF8, (BYTE*)&patched_health_value, sizeof(patched_health_value), process);
			}

			if (settings.unlimited_armor) {
				const int32_t patched_armor_value = 1338;
				mem::PatchBytes((BYTE*)local_player_address + 0xFC, (BYTE*)&patched_armor_value, sizeof(patched_armor_value), process);
			}

			// Start frame
			gui::StartFrame();

			// Gui stuff
			gui::ShowOptions(settings);
			gui::ShowLocalPlayerInformation(local_player);
			gui::ShowPlayerInformation(players, settings.fov);

			// Clear and sleep until next cycle
			players.clear();

			Sleep(10);

			// Render
			gui::Render();
		}

		gui::Cleanup(window_handle);

		return 0;
	} else {
		std::cout << "Process ID for \"ac_client.exe\" not found" << std::endl;

		return 1;
	}

}

void CalculateYawAndPitchToOtherPlayer(const std::array<float, 3>& local_player_position, const std::array<float, 3>& target_position, float& yaw, float& pitch) {
	std::array<float, 3> target_relative_to_head = {};
	vec::Subtract(target_position, local_player_position, target_relative_to_head);

	float local_yaw = fmod(atan2(target_relative_to_head[1], target_relative_to_head[0]) * 180.0f / kPi - 270.0f, 360.0f);
	if (local_yaw < 0) local_yaw += 360.0f;

	float tmp = sqrt(target_relative_to_head[0] * target_relative_to_head[0] + target_relative_to_head[1] * target_relative_to_head[1]);
	float local_pitch = atan2(-target_relative_to_head[2], tmp) * 180.0f / kPi;
	local_pitch = -local_pitch;

	yaw = local_yaw;
	pitch = local_pitch;
}

// TODO: implement correct head offset
std::array<float, 3> CorrectHeadPosition(const PlayerEntity& player) {
	std::array<float, 3> corrected_head = player.position_head;
	corrected_head[2] += 0.2f;

	return corrected_head;
}
