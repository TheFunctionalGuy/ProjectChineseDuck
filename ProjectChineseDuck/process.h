#ifndef ACE_PROCESS_H_
#define ACE_PROCESS_H_

#include <Windows.h>

#include <vector>

DWORD GetProcessId(const wchar_t* const process_name);
uintptr_t GetModuleBaseAddress(const DWORD process_id, const wchar_t* const module_name);
uintptr_t FindDMAAddress(const HANDLE process, const uintptr_t base_pointer, const std::vector<unsigned int>& offsets);

#endif // ACE_PROCESS_H_
