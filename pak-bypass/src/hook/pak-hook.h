#pragma once

#include <windows.h>
#include <cstdint>

namespace PakHook {
    bool Init();
    void Cleanup();
    bool IsInitialized();
}
