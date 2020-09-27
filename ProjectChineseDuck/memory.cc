#include "memory.h"

void mem::PatchBytes(BYTE* const dst, const BYTE* src, const unsigned int size, const HANDLE process) {
	DWORD old_protect;

	VirtualProtectEx(process, dst, size, PAGE_EXECUTE_READWRITE, &old_protect);
	WriteProcessMemory(process, dst, src, size, nullptr);
	VirtualProtectEx(process, dst, size, old_protect, &old_protect);
}

void mem::NopBytes(BYTE* const dst, const unsigned int size, const HANDLE process) {
	BYTE* nop_array = new BYTE[size];
	memset(nop_array, 0x90, size);

	PatchBytes(dst, nop_array, size, process);
	delete[] nop_array;
}
