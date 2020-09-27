#ifndef ACE_EXTENDED_PLAYER_ENTITY_H_
#define ACE_EXTENDED_PLAYER_ENTITY_H_

#include <array>

#include "player_entity.h"

struct ExtendedPlayerEntity {
	PlayerEntity player;
	float distance_to_local_player;
	std::array<float, 2> angles_from_local_player;
	std::array<float, 2> fov_diff;
};

#endif // ACE_EXTENDED_PLAYER_ENTITY_H_
