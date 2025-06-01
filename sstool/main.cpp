#include "PatternScan.h"

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

    auto handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);

    //if a handle isn't open dont continue and let them enter a proper PID
    if (!handle) {
        std::cout << "\n Invalid Process.\n";
        main();
    } else {
        std::cout << "Select Client To Scan For: \n";

        for (int i = 0; i < clientList.size(); i++)
            std::cout << (i + 1) << ". " << clientList.at(i) << "\n";

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