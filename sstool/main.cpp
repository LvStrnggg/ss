#include <Windows.h>
#include <string>
#include <iostream>
#include <vector>
#include <string_view>

std::vector<void*> pattern_scan(HANDLE hProcess, const std::vector<std::string_view>& patterns);

std::vector<std::string_view> novaPatterns = {
    "aHR0cDovL2FwaS5ub3ZhY2xpZW50LmxvbC93ZWJob29rLnR4dA==",
    "novaclient",
    "addFri",
    "antiAttack",
    "/assets/font/font.ttf", //also works for argon
    "Lithium is not initialized! Skipping event: ",
    "Error in hash"
};

std::vector<std::string_view> universalPatterns = {
    "Auto Crystal",
    "Self Destruct",
    "Auto Anchor",
    "Auto Loot Yeeter",
    "CwCrystal.class",
    "ADH.class",
    "ModuleManager.class"
};

std::vector<std::string> clientList = {
    "Nova Client",
    "All (Might not work for all clients)"
};

int main() {
    std::cout << "Screenshare tool | Made by lvstrng | v1.0\n";

    DWORD pid;
    std::cout << "Minecraft PID:";
    std::cin >> pid;

    auto handle = OpenProcess(PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION, FALSE, pid);

    //if a handle isn't open dont continue and let them enter a proper PID
    if (!handle) {
        std::cout << "\n Invalid Process.\n";
        main();
    } else {
        std::cout << "Select Client To Scan For: \n";

        for (int i = 0; i < clientList.size(); i++) {
            std::cout << (i + 1) << ". " << clientList.at(i) << "\n";
        }

        int option;
        std::cin >> option;

        //switch because we will need this for other clients lol
        std::vector<std::string_view> scannable;
        switch (option) {
            case 1: {
                    scannable = novaPatterns;
                    break;
            }
            case 2: {
                    scannable = universalPatterns;
                    break;
            }
        }

        auto results = pattern_scan(handle, scannable);

        std::cout << "Found " << std::dec << results.size() << " traces \n";
    }

    std::cout << "Press any key to exit...\n";
    std::cin.ignore();
    std::cin.get();

	return 0;
}

std::vector<void*> pattern_scan(HANDLE hProcess, const std::vector<std::string_view>& patterns) {
    SYSTEM_INFO sys_info;
    GetSystemInfo(&sys_info);

    std::vector<void*> results;
    MEMORY_BASIC_INFORMATION memInfo;
    uint8_t* address = static_cast<uint8_t*>(sys_info.lpMinimumApplicationAddress);

    while (address < sys_info.lpMaximumApplicationAddress && VirtualQueryEx(hProcess, address, &memInfo, sizeof(memInfo))) {
        if (memInfo.State == MEM_COMMIT && (memInfo.Protect & (PAGE_READWRITE | PAGE_EXECUTE_READWRITE)) &&
            memInfo.Type == MEM_PRIVATE) {

            std::vector<uint8_t> buffer(memInfo.RegionSize);
            SIZE_T bytesRead;
            if (ReadProcessMemory(hProcess, memInfo.BaseAddress, buffer.data(), buffer.size(), &bytesRead)) {
                std::string_view view(reinterpret_cast<char*>(buffer.data()), bytesRead);

                for (const auto& pattern : patterns) {
                    size_t pos = 0;
                    while ((pos = view.find(pattern, pos)) != std::string_view::npos) {
                        void* found = static_cast<uint8_t*>(memInfo.BaseAddress) + pos;
                        std::cout << "[*] Found string " << pattern << " at " << std::hex << std::uppercase << reinterpret_cast<uintptr_t>(found) << "\n";
                        results.push_back(found);
                        ++pos;
                    }
                }
            }
        }
        address = static_cast<uint8_t*>(memInfo.BaseAddress) + memInfo.RegionSize;
    }
    return results;
}
