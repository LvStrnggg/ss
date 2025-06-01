#pragma once
#include <Windows.h>
#include <string>
#include <iostream>
#include <vector>
#include <string_view>

std::vector<void*> pattern_scan(HANDLE hProcess, const std::vector<std::string_view>& patterns) {
    SYSTEM_INFO sys_info;
    GetSystemInfo(&sys_info);

    std::vector<void*> results;
    MEMORY_BASIC_INFORMATION memInfo;
    byte* address = reinterpret_cast<byte*>(sys_info.lpMinimumApplicationAddress);

    while (address < sys_info.lpMaximumApplicationAddress && VirtualQueryEx(hProcess, address, &memInfo, sizeof(memInfo))) {
        if (memInfo.State == MEM_COMMIT && (memInfo.Protect & (PAGE_READWRITE | PAGE_EXECUTE_READWRITE)) &&
            memInfo.Type == MEM_PRIVATE) {

            std::vector<byte> buffer(memInfo.RegionSize);
            SIZE_T bytesRead;
            if (ReadProcessMemory(hProcess, memInfo.BaseAddress, buffer.data(), buffer.size(), &bytesRead)) {
                std::string_view view(reinterpret_cast<char*>(buffer.data()), bytesRead);

                for (const auto& pattern : patterns) {
                    size_t pos = 0;
                    while ((pos = view.find(pattern, pos)) != std::string_view::npos) {
                        void* found = reinterpret_cast<byte*>(memInfo.BaseAddress) + pos;
                        std::cout << "[*] Found string " << pattern << " at " << std::hex << std::uppercase << reinterpret_cast<uintptr_t>(found) << "\n";
                        results.push_back(found);
                        ++pos;
                    }
                }
            }
        }
        address = reinterpret_cast<byte*>(memInfo.BaseAddress) + memInfo.RegionSize;
    }
    return results;
}
