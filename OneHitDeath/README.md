# OneHitDeath - Elden Ring Mod

Locks your max HP to 1, making every hit lethal.
Based on Hexinton's All-in-One Cheat Table v5.0 pointer offsets.

## Requirements

- Visual Studio 2022 (v143 toolset)
- Windows SDK 10.0
- No external libraries needed — ModUtils.h is self-contained

## Build Instructions

1. Open `OneHitDeath.sln` in Visual Studio 2022
2. Set configuration to **Release | x64**
3. Set the PostBuild copy destination:
   - Either set environment variable `ELDEN_RING_MODS_PATH` to your mods folder path
   - Or edit the PostBuildEvent in OneHitDeath.vcxproj directly:
     `COPY "$(OutDir)$(ProjectName).dll" "F:\SteamLibrary\steamapps\common\ELDEN RING\Game\mods\"`
4. Build (Ctrl+Shift+B)

## Installation

- Drop `OneHitDeath.dll` into `<Elden Ring>\Game\mods\`
- Requires Elden Ring Mod Loader (elden-ring-mod-loader by techiew on GitHub)

## References / Linker Dependencies

The only non-standard library used is `Psapi.lib` which ships with the Windows SDK.
It is already listed in the vcxproj AdditionalDependencies for Release|x64.
No NuGet packages or external SDKs are required.

## Pointer Chain

WorldChrMan (AOB) -> [+0x1E508] -> [+0x190] -> [+0x0] -> HP @ +0x138 / MaxHP @ +0x13C, +0x140

## Log File

A log file `OneHitDeath.log` is written next to `eldenring.exe` on startup.
Check it if the mod isn't working — it will tell you if the AOB scan failed.
