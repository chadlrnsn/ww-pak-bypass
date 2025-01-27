#include "pak-hook.h"
#include <logger.h>
#include <PatternScan.h>
#include <minhook/include/MinHook.h>
#include <string>
#include <thread>
#include <chrono>


namespace {
    bool isInitialized = false;
    uintptr_t hookAddress = 0;
    void* originalFunction = nullptr;

    void HookFunction(uintptr_t rcx) {
        const wchar_t* pPakName = *(const wchar_t**)(rcx + 8);
        std::wstring pakName(pPakName);
        LOG_SUCCESS("Processing pak: %ws", pakName.c_str());
        return;
    }

    bool FindAndHook() {
        if (GetModuleHandleA(NULL)) {
            hookAddress = Memory::FindPattern(
                GetModuleHandleA(NULL), 
                "40 55 56 57 41 57 48 8d 6c 24 ? 48 81 ec ? ? ? ? 80 3d"
            );
            
            if (hookAddress) {
                LOG_SUCCESS("Found pattern at: %p", hookAddress);
                return true;
            }
        }
        return false;
    }
}

namespace PakHook {
    bool IsInitialized() { 
        return isInitialized; 
    }

    bool Init() {
        Logger::Init("pak-bypass");

        if (MH_Initialize() != MH_OK) {
            LOG_ERROR("Failed to initialize MinHook");
            return false;
        }

        while (!hookAddress) {
            if (!FindAndHook()) {
                LOG_WARNING("Pattern not found, waiting for module to load...");
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
        }

        if (MH_CreateHook(
            (LPVOID)hookAddress, 
            (LPVOID)HookFunction, 
            &originalFunction) != MH_OK) {
            LOG_ERROR("Failed to create hook");
            return false;
        }

        if (MH_EnableHook((LPVOID)hookAddress) != MH_OK) {
            LOG_ERROR("Failed to enable hook");
            return false;
        }

        LOG_SUCCESS("Hook initialized successfully");
        isInitialized = true;
        return true;
    }

    void Cleanup() {
        if (!isInitialized) return;

        if (MH_DisableHook((LPVOID)hookAddress) != MH_OK) {
            LOG_ERROR("Failed to disable hook");
        }

        if (MH_RemoveHook((LPVOID)hookAddress) != MH_OK) {
            LOG_ERROR("Failed to remove hook");
        }

        if (MH_Uninitialize() != MH_OK) {
            LOG_ERROR("Failed to uninitialize MinHook");
        }

        LOG_SUCCESS("Hook removed successfully");
        isInitialized = false;
    }
}
