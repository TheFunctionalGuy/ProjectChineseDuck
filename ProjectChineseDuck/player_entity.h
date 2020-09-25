#ifndef ACE_PLAYER_ENTITY_H_
#define ACE_PLAYER_ENTITY_H_

#include "process.h"
#include <cstdint>
#include <array>

class PlayerEntity {
public:
	char pad_0000[4];
	std::array<float, 3> position_head; //0x0004
	char pad_0010[36]; //0x0010
	std::array<float, 3> position; //0x0034
	std::array<float, 3> angles; //0x0040
	char pad_004C[172]; //0x004C
	int32_t health; //0x00F8
	int32_t armor; //0x00FC
	char pad_0100[293]; //0x0100
	char name[16]; //0x0225
	char pad_0235[247]; //0x0235
	int32_t team; //0x032C
	char pad_0330[8]; //0x0330
	int32_t is_dead; //0x0338
}; //Size: 0x033C

#endif // ACE_PLAYER_ENTITY_H_