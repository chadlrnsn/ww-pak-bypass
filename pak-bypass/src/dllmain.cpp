#include <iostream>
#include <thread>

#include "logger.h"

#include "../ext/minhook/MinHook.h"

/*
Search for "Checking pak file "%s". This may take a while..." in a disassembler

Or make an actual signature:
Client-Win64-Shipping.exe+3FF8E52 - 56                    - push rsi
Client-Win64-Shipping.exe+3FF8E53 - 57                    - push rdi
Client-Win64-Shipping.exe+3FF8E54 - 41 57                 - push r15
Client-Win64-Shipping.exe+3FF8E56 - 48 8D 6C 24 C1        - lea rbp,[rsp-3F]
Client-Win64-Shipping.exe+3FF8E5B - 48 81 EC E8000000     - sub rsp,000000E8
Client-Win64-Shipping.exe+3FF8E62 - 80 3D 5F464004 04     - cmp byte ptr [Client-Win64-Shipping.exe+83FD4C8],04
Client-Win64-Shipping.exe+3FF8E69 - 48 8D 3D 58385202     - lea rdi,[Client-Win64-Shipping.exe+651C6C8]
Client-Win64-Shipping.exe+3FF8E70 - 0F29 BC 24 B0000000   - movaps [rsp+000000B0],xmm7
Client-Win64-Shipping.exe+3FF8E78 - 48 8B F1              - mov rsi,rcx
Client-Win64-Shipping.exe+3FF8E7B - 72 36                 - jb Client-Win64-Shipping.exe+3FF8EB3
Client-Win64-Shipping.exe+3FF8E7D - 83 79 10 00           - cmp dword ptr [rcx+10],00
Client-Win64-Shipping.exe+3FF8E81 - 74 06                 - je Client-Win64-Shipping.exe+3FF8E89
*/
const uintptr_t checkPak = 0x3FF8E50;

typedef int(__fastcall* fnPakCheck)(uintptr_t rcx);
fnPakCheck oPakCheck = nullptr;

int __fastcall hkCheckPak(uintptr_t rcx) {
    // I straight up copied this lol
    const wchar_t* pPakName = *(const wchar_t**)(rcx + 8);
    std::wstring pakName(pPakName);
    LOG_SUCCESS("Processing pak: %ws", pakName.c_str());
    return 1;
}

void initHook() {
    if (MH_Initialize() != MH_OK) {
        LOG_ERROR("Failed to initialize MinHook");
        return;
    }

    uintptr_t baseAddress = (uintptr_t)GetModuleHandleA("Client-Win64-Shipping.exe");
    uintptr_t checkPakAddress = baseAddress + checkPak;

    LOG_SUCCESS("checkPak: %p", checkPakAddress);

    if (MH_CreateHook((LPVOID)checkPakAddress, (LPVOID)hkCheckPak, reinterpret_cast<LPVOID*>(&oPakCheck)) != MH_OK) {
        LOG_ERROR("Failed to create hook");
        return;
    }

    if (MH_EnableHook((LPVOID)checkPakAddress) != MH_OK) {
        LOG_ERROR("Failed to create hook");
        return;
    }

    LOG_SUCCESS("Hooked successfully");
}

DWORD WINAPI mainThread(LPVOID) {
    Logger::Init("Log");

    // Ensure game is loaded since it would just rewrite the hook
    while (!FindWindowA("UnrealWindow", 0)) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    initHook();

    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID lpReserved) {
    DisableThreadLibraryCalls(hModule);

    if (reason == DLL_PROCESS_ATTACH) {
        CreateThread(nullptr, 0, mainThread, nullptr, 0, nullptr);
    }

    return TRUE;
}
