#include <iostream>
#include <algorithm>
#include <array>

#include "process.h"
#include "memory.h"
#include "player_entity.h"
#include "vector.h"
#include "gui.h"

int main() {
	// Init GUI
	if (!gui::InitGui()) {
		std::cout << "There was an error initializing the GUI." << std::endl;

		return 1;
	}

	// Fov
	float fov = 90;

	// Base addresses
	uintptr_t module_base_address = 0;
	uintptr_t player_count_address = 0;
	uintptr_t local_player_base = 0;
	uintptr_t entity_list_base = 0;

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
		std::vector<PlayerEntity> players = {};

		// Switches
		bool aimbot = false;

		// Main loop
		while (true) {
			// Get player count
			//uintptr_t player_count_address = FindDMAAddress(process, player_count_base, { 0x0 });
			ReadProcessMemory(process, (BYTE*)player_count_address, &player_count, sizeof(player_count), nullptr);

			// Get local player
			uintptr_t local_player_address = FindDMAAddress(process, local_player_base, { 0x0 });
			ReadProcessMemory(process, (BYTE*)local_player_address, &local_player, sizeof(local_player), nullptr);

			// Print local player position and angles
			std::cout << "Player position:" << std::endl;
			std::cout << vec::ToString(local_player.position) << std::endl;

			std::cout << "Player angles:" << std::endl;
			std::cout << vec::ToString(local_player.angles) << std::endl;
			
			// Get players
			for (unsigned int i = 1; i < player_count; i++) {
				uintptr_t player_address = FindDMAAddress(process, entity_list_base, { i * 0x4, 0x0 });
				PlayerEntity player;

				ReadProcessMemory(process, (BYTE*)player_address, &player, sizeof(player), nullptr);

				players.insert(players.begin(), player);
			};

			// TODO: Check if distance calculations are right. They seem a little bit of some times.
			auto sortRuleLambda = [&](PlayerEntity player1, PlayerEntity player2) -> bool {
				return vec::Dist(local_player.position_head, player1.position_head) < vec::Dist(local_player.position_head, player2.position_head);
			};

			// Sort array by distance to local player
			std::vector<PlayerEntity> players_alive;

			std::sort(players.begin(), players.end(), sortRuleLambda);
			std::copy_if(players.begin(), players.end(), std::back_inserter(players_alive), [](PlayerEntity player) {return !player.is_dead; });

			std::cout << "Bots (" << players.size() << "):" << std::endl;
			for (const auto& player : players) {
				std::cout << "Alive: " << !player.is_dead << " " << vec::ToString(player.position) << " " << player.name << std::endl;
			}

			if (GetAsyncKeyState(VK_END) & 1) {
				std::cout << "Program exited." << std::endl;

				return 0;
			}

			// Aimbot active
			if (GetAsyncKeyState(VK_LCONTROL)) {
				// Calculate yaw and pitch
				std::array<float, 3> target_relative_to_head = {};

				for (const auto& player : players_alive) {
					vec::Subtract(player.position_head, local_player.position_head, target_relative_to_head);

					float yaw = atan2(target_relative_to_head[1], target_relative_to_head[0]) * 180.0 / 3.141592653589793238463 + 90.0;
					if (yaw < 0) yaw += 360;
					std::cout << "Yaw: " << yaw << std::endl;

					float tmp = sqrt(target_relative_to_head[0] * target_relative_to_head[0] + target_relative_to_head[1] * target_relative_to_head[1]);
					float pitch = atan2(-target_relative_to_head[2], tmp) * 180.0 / 3.141592653589793238463;
					pitch = -pitch;
					std::cout << "Pitch: " << pitch << std::endl;

					if (std::abs(yaw - local_player.angles[0]) < fov / 2 && std::abs(pitch - local_player.angles[1]) < (fov * (16 / 9)) / 2) {
						uintptr_t angles_address = FindDMAAddress(process, local_player_base, { 0x40 });

						mem::PatchBytes((BYTE*)angles_address, (BYTE*)&yaw, sizeof(yaw), process);
						mem::PatchBytes((BYTE*)angles_address + 4, (BYTE*)&pitch, sizeof(pitch), process);

						break;
					}
				}
			}

			// Clear and sleep until next cycle
			players.clear();

			Sleep(10);
		}

		return 0;
	} else {
		std::cout << "Process ID for \"ac_client.exe\" not found" << std::endl;

		return 1;
	}

}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
