#include "process.h"
#include <TlHelp32.h>

DWORD GetProcessId(const wchar_t* process_name) {
	DWORD process_id = 0;
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (snapshot != INVALID_HANDLE_VALUE) {
		PROCESSENTRY32 process_entry;
		process_entry.dwSize = sizeof(process_entry);

		if (Process32First(snapshot, &process_entry)) {
			do {
				if (_wcsicmp(process_entry.szExeFile, process_name) == 0) {
					process_id = process_entry.th32ProcessID;
					break;
				}
			} while (Process32Next(snapshot, &process_entry));
		}
	}

	CloseHandle(snapshot);

	return process_id;
}

uintptr_t GetModuleBaseAddress(const DWORD process_id, const wchar_t* module_name) {
	uintptr_t module_base_address = 0;
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, process_id);

	if (snapshot != INVALID_HANDLE_VALUE) {
		MODULEENTRY32 module_entry;
		module_entry.dwSize = sizeof(module_entry);

		if (Module32First(snapshot, &module_entry)) {
			do {
				if (_wcsicmp(module_entry.szModule, module_name) == 0) {
					module_base_address = (uintptr_t)module_entry.modBaseAddr;
					break;
				}
			} while (Module32Next(snapshot, &module_entry));
		}
	}

	CloseHandle(snapshot);

	return module_base_address;
}

uintptr_t FindDMAAddress(const HANDLE process, const uintptr_t base_pointer, const std::vector<unsigned int> offsets) {
	uintptr_t resolved_address = base_pointer;

	for (unsigned int i = 0; i < offsets.size(); i++) {
		ReadProcessMemory(process, (BYTE*)resolved_address, &resolved_address, sizeof(resolved_address), nullptr);
		resolved_address += offsets[i];
	}

	return resolved_address;
}