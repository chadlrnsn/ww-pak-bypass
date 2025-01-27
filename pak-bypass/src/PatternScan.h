#pragma once

#include <windows.h>
#include <vector>
#include <string>
#include <cstdint>
#include <psapi.h>
#include <stdio.h>

#include <logger.h>

class Memory {
public:
    static uintptr_t FindPattern(HMODULE module, const char* pattern) {
        MODULEINFO modInfo;
        if (!GetModuleInformation(GetCurrentProcess(), module, &modInfo, sizeof(MODULEINFO))) {
            LOG_ERROR("GetModuleInformation failed with error: %lu\n", GetLastError());
            return 0;
        }

        LOG_INFO("Module base address: %p\n", module);
        LOG_INFO("Module size: %zu\n", modInfo.SizeOfImage);

        uint8_t* scanStart = reinterpret_cast<uint8_t*>(module);
        size_t scanSize = modInfo.SizeOfImage;
        
        // Check valid memory
        MEMORY_BASIC_INFORMATION mbi;
        if (VirtualQuery(scanStart, &mbi, sizeof(mbi))) {
            LOG_INFO("Memory protection: %lu\n", mbi.Protect);
            LOG_INFO("Memory state: %lu\n", mbi.State);
        }

        std::vector<std::pair<uint8_t, bool>> bytes; // byte and flag (true if not wildcard)
        
        // Parsing pattern
        for (const char* ptr = pattern; *ptr; ptr++) {
            if (*ptr == ' ' || *ptr == '?') {
                if (*ptr == '?' && *(ptr + 1) != ' ') ptr++; // Skip second '?' if exists
                if (*ptr == '?') bytes.push_back({0, false}); // Wildcard
                continue;
            }

            char byte = 0;
            for (int i = 0; i < 2; i++) {
                if (ptr[i] >= '0' && ptr[i] <= '9')
                    byte = (byte << 4) | (ptr[i] - '0');
                else if (ptr[i] >= 'A' && ptr[i] <= 'F')
                    byte = (byte << 4) | (ptr[i] - 'A' + 10);
                else if (ptr[i] >= 'a' && ptr[i] <= 'f')
                    byte = (byte << 4) | (ptr[i] - 'a' + 10);
            }
            bytes.push_back({byte, true});
            ptr++; // Skip second byte symbol
        }

        LOG_INFO("Pattern bytes: ");
        for (const auto& b : bytes) {
            if (b.second)
                LOG_INFO("%02X ", b.first);
            else
                LOG_INFO("?? ");
        }
        LOG_INFO("\n");

        size_t patternLength = bytes.size();
        LOG_INFO("patternLength: %zu\n", patternLength);

        for (size_t i = 0; i < scanSize - patternLength; i++) {
            bool found = true;
            for (size_t j = 0; j < patternLength; j++) {
                if (bytes[j].second && scanStart[i + j] != bytes[j].first) {
                    found = false;
                    break;
                }
            }
            if (found) {
                LOG_INFO("Pattern found at: %p\n", (void*)(scanStart + i));
                return reinterpret_cast<uintptr_t>(scanStart + i);
            }
        }
        
        return 0;
    }
};
