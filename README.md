# sonmat-loader

A part of 손맛 햅틱 / Sonmat Haptics (`sonmat`): dshaptics_loader.dll, dshaptics_loader_dinput8.dll.

The loader: a proxy DLL under the name of a system DLL the game's exe loads from its own folder (version.dll, or dinput8.dll for games that never load version.dll). It forwards every call to the real one in System32 and loads the hook DLLs listed in dshaptics_load.txt beside it - no injection, no admin rights. `dshaptics_loader_x86.dll` is the same version.dll loader for 32-bit games (Batman: Arkham Asylum).

The app does not carry this in its installer for good: it fetches the release zip that `sonmat-games/parts.json` names
(address + SHA-256) into `<app>/parts/loader/` when a game needs it (`sonmat/tools/partfetch.py`). A change here reaches
players when a new tag has been released and `parts.json` points at it.

Build: Visual Studio C++ tools (x64): build.bat dshaptics_loader / build.bat dshaptics_loader_dinput8. Every push to main builds it on GitHub (artifact); a tag `v*` makes the release.
