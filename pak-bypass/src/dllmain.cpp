#include <thread>
#include <logger.h>
#include <hook/pak-hook.h>

DWORD WINAPI mainThread(LPVOID lpReserved, HMODULE hModule) {
    Logger::Init("Log");

    // Ensure game is loaded
    while (!FindWindowA("UnrealWindow", 0)) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    if (!PakHook::Init()) {
        LOG_ERROR("Failed to initialize hook");
        return 1;
    }

    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));

        if (GetAsyncKeyState(VK_END) & 0x8000) {
            break;
        }
    }
    
    PakHook::Cleanup();

    // Wait for cleanup
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    FreeLibraryAndExitThread(hModule, 0);
    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID lpReserved) {
    DisableThreadLibraryCalls(hModule);

    switch (reason) {
        case DLL_PROCESS_ATTACH:
            CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)mainThread, hModule, 0, nullptr);
            break;
        case DLL_PROCESS_DETACH:
            PakHook::Cleanup();
            break;
    }

    return TRUE;
}
