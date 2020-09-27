#ifndef ACE_EXTENDED_PLAYER_ENTITY_H_
#define ACE_EXTENDED_PLAYER_ENTITY_H_

#include <array>

#include "player_entity.h"

class ExtendedPlayerEntity {
public:
	PlayerEntity player;
	float distance_to_local_player;
	std::array<float, 2> angles_from_local_player;
	std::array<float, 2> fov_diff;
	
	ExtendedPlayerEntity(const PlayerEntity player, const float distance_to_local_player, std::array<float, 2> angles_from_local_player, std::array<float, 2> fov_diff) {
		this->player = player;
		this->distance_to_local_player = distance_to_local_player;
		this->angles_from_local_player = angles_from_local_player;
		this->fov_diff = fov_diff;
	}
};

#endif // ACE_EXTENDED_PLAYER_ENTITY_H_
