#ifndef ACE_MEMORY_H_
#define ACE_MEMORY_H_

#include <Windows.h>

namespace mem {
	void PatchBytes(BYTE* const dst, const BYTE* src, const unsigned int size, const HANDLE process);
	void NopBytes(BYTE* const dst, const unsigned int size, const HANDLE process);
}

#endif // ACE_MEMORY_H_
