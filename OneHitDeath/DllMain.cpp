#include <Windows.h>
#include "ModUtils.h"
using namespace ModUtils;

// WorldChrMan AOB - MOV RAX, [RIP+offset] pattern
// 48 8B 05 ?? ?? ?? ?? 48 85 C0 74 0F 48 39 88
std::string WORLD_CHR_MAN_AOB = "48 8b 05 ? ? ? ? 48 85 c0 74 0f 48 39 88";

// Pointer chain offsets (from Hexinton's cheat table)
const uintptr_t LOCAL_PLAYER_OFFSET = 0x1E508;
const uintptr_t OFFSET_190 = 0x190;
const uintptr_t HP_OFFSET = 0x138;
const uintptr_t MAX_HP_OFFSET = 0x13C;
const uintptr_t MAX_HP_OFFSET_2 = 0x140;
const int       LOCKED_HP = 1;

bool g_modEnabled = true;

uintptr_t ResolveWorldChrMan()
{
    uintptr_t sigAddr = AobScan(WORLD_CHR_MAN_AOB);
    if (sigAddr == 0) return 0;

    uintptr_t worldChrMan = RelativeToAbsoluteAddress(sigAddr + 3);
    Log("WorldChrMan resolved at: ", NumberToHexString(worldChrMan));
    return worldChrMan;
}

uintptr_t GetPlayerBase(uintptr_t worldChrManAddr)
{
    uintptr_t worldChrManPtr = *(uintptr_t*)worldChrManAddr;
    if (worldChrManPtr == 0) return 0;

    uintptr_t playerPtr = *(uintptr_t*)(worldChrManPtr + LOCAL_PLAYER_OFFSET);
    if (playerPtr == 0) return 0;

    uintptr_t offset190 = *(uintptr_t*)(playerPtr + OFFSET_190);
    if (offset190 == 0) return 0;

    uintptr_t playerBase = *(uintptr_t*)offset190;
    return playerBase;
}

DWORD WINAPI MainThread(LPVOID lpParam)
{
    Log("OneHitDeath loading...");

    while (g_modEnabled)
    {
        // STAGE 1: Keep retrying AOB scan until WorldChrMan is found
        uintptr_t worldChrManAddr = 0;
        while (g_modEnabled && worldChrManAddr == 0)
        {
            worldChrManAddr = ResolveWorldChrMan();
            if (worldChrManAddr == 0)
            {
                Log("WorldChrMan not found, retrying in 1s...");
                Sleep(1000);
            }
        }

        if (!g_modEnabled) break;
        Log("WorldChrMan found. Entering HP lock loop.");

        // STAGE 2: Non-stop HP lock loop
        // - WorldChrMan ptr is null (menu/loading): spin and wait
        // - Player pointer chain is null (dead/loading): spin and wait
        // - Player valid: hammer HP=1 every tick unconditionally, no skip guards
        //   so respawn HP reset is caught on the very first tick it's readable
        while (g_modEnabled)
        {
            uintptr_t worldChrManPtr = *(uintptr_t*)worldChrManAddr;
            if (worldChrManPtr == 0)
            {
                Sleep(50);
                continue;
            }

            uintptr_t playerBase = GetPlayerBase(worldChrManAddr);
            if (playerBase != 0)
            {
                ToggleMemoryProtection(false, playerBase + MAX_HP_OFFSET, 8);
                *(int*)(playerBase + MAX_HP_OFFSET) = LOCKED_HP;
                *(int*)(playerBase + MAX_HP_OFFSET_2) = LOCKED_HP;
                ToggleMemoryProtection(true, playerBase + MAX_HP_OFFSET, 8);

                ToggleMemoryProtection(false, playerBase + HP_OFFSET, 4);
                *(int*)(playerBase + HP_OFFSET) = LOCKED_HP;
                ToggleMemoryProtection(true, playerBase + HP_OFFSET, 4);
            }

            Sleep(50);
        }
    }

    Log("OneHitDeath unloaded.");
    CloseLog();
    return 0;
}

BOOL WINAPI DllMain(HINSTANCE module, DWORD reason, LPVOID)
{
    if (reason == DLL_PROCESS_ATTACH)
    {
        DisableThreadLibraryCalls(module);
        CreateThread(0, 0, &MainThread, 0, 0, NULL);
    }
    if (reason == DLL_PROCESS_DETACH)
    {
        g_modEnabled = false;
    }
    return 1;
}