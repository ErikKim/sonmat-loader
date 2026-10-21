// dshaptics_loader - a version.dll proxy that loads the hook DLL with the game, instead of injecting it.
//
// Installed as <game>\Binaries\Win64\version.dll (tools/install_loader.py). The game loads it from its
// own folder (version.dll is not a KnownDLL), every export is forwarded to the real System32 copy, and
// on start it LoadLibrary()s each path listed in dshaptics_load.txt next to it (one absolute path per
// line, '#' comments). No admin rights, no remote thread - the same route UE4SS takes with dwmapi.dll.
//
// build: build.bat dshaptics_loader
// The same loader as dinput8.dll (dshaptics_loader_dinput8.cpp defines PROXY_DINPUT8): NieR and Sekiro never load
// version.dll but do load dinput8.dll from their own folder (3050 Ti, 10-18: no hook for 45 s, then injection)
#include <windows.h>
#include <string>

#ifdef PROXY_DINPUT8
#pragma comment(linker, "/export:DirectInput8Create=C:\\Windows\\System32\\dinput8.DirectInput8Create")
#pragma comment(linker, "/export:DllCanUnloadNow=C:\\Windows\\System32\\dinput8.DllCanUnloadNow,PRIVATE")
#pragma comment(linker, "/export:DllGetClassObject=C:\\Windows\\System32\\dinput8.DllGetClassObject,PRIVATE")
#pragma comment(linker, "/export:DllRegisterServer=C:\\Windows\\System32\\dinput8.DllRegisterServer,PRIVATE")
#pragma comment(linker, "/export:DllUnregisterServer=C:\\Windows\\System32\\dinput8.DllUnregisterServer,PRIVATE")
#pragma comment(linker, "/export:GetdfDIJoystick=C:\\Windows\\System32\\dinput8.GetdfDIJoystick")
#else
#pragma comment(linker, "/export:GetFileVersionInfoA=C:\\Windows\\System32\\version.GetFileVersionInfoA")
#pragma comment(linker, "/export:GetFileVersionInfoByHandle=C:\\Windows\\System32\\version.GetFileVersionInfoByHandle")
#pragma comment(linker, "/export:GetFileVersionInfoExA=C:\\Windows\\System32\\version.GetFileVersionInfoExA")
#pragma comment(linker, "/export:GetFileVersionInfoExW=C:\\Windows\\System32\\version.GetFileVersionInfoExW")
#pragma comment(linker, "/export:GetFileVersionInfoSizeA=C:\\Windows\\System32\\version.GetFileVersionInfoSizeA")
#pragma comment(linker, "/export:GetFileVersionInfoSizeExA=C:\\Windows\\System32\\version.GetFileVersionInfoSizeExA")
#pragma comment(linker, "/export:GetFileVersionInfoSizeExW=C:\\Windows\\System32\\version.GetFileVersionInfoSizeExW")
#pragma comment(linker, "/export:GetFileVersionInfoSizeW=C:\\Windows\\System32\\version.GetFileVersionInfoSizeW")
#pragma comment(linker, "/export:GetFileVersionInfoW=C:\\Windows\\System32\\version.GetFileVersionInfoW")
#pragma comment(linker, "/export:VerFindFileA=C:\\Windows\\System32\\version.VerFindFileA")
#pragma comment(linker, "/export:VerFindFileW=C:\\Windows\\System32\\version.VerFindFileW")
#pragma comment(linker, "/export:VerInstallFileA=C:\\Windows\\System32\\version.VerInstallFileA")
#pragma comment(linker, "/export:VerInstallFileW=C:\\Windows\\System32\\version.VerInstallFileW")
#pragma comment(linker, "/export:VerLanguageNameA=C:\\Windows\\System32\\version.VerLanguageNameA")
#pragma comment(linker, "/export:VerLanguageNameW=C:\\Windows\\System32\\version.VerLanguageNameW")
#pragma comment(linker, "/export:VerQueryValueA=C:\\Windows\\System32\\version.VerQueryValueA")
#pragma comment(linker, "/export:VerQueryValueW=C:\\Windows\\System32\\version.VerQueryValueW")
#endif

static DWORD WINAPI load(LPVOID self) {
    wchar_t path[MAX_PATH]; GetModuleFileNameW((HMODULE)self, path, MAX_PATH);
    std::wstring dir(path); dir = dir.substr(0, dir.find_last_of(L"\\/") + 1);
    FILE* f = _wfopen((dir + L"dshaptics_load.txt").c_str(), L"r, ccs=UTF-8");
    if (!f) return 0;
    wchar_t line[1024];
    while (fgetws(line, 1024, f)) {
        std::wstring s(line);
        s.erase(s.find_last_not_of(L" \t\r\n") + 1);
        s.erase(0, s.find_first_not_of(L" \t\ufeff"));
        if (s.empty() || s[0] == L'#') continue;
        LoadLibraryW(s.c_str());           // each hook DLL starts its own init thread
    }
    fclose(f);
    return 0;
}

BOOL APIENTRY DllMain(HMODULE h, DWORD reason, LPVOID) {
    if (reason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(h);
        CreateThread(nullptr, 0, load, h, 0, nullptr);   // never LoadLibrary under the loader lock
    }
    return TRUE;
}
