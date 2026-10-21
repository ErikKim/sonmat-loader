@echo off
rem Build one DLL with the Visual Studio C++ tools (x64).
rem   build.bat <name>      (<name>.cpp / <name>.c -> <name>.dll)
setlocal
cd /d %~dp0
set NAME=%1
for /f "usebackq delims=" %%i in (`"%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe" -latest -products * -property installationPath`) do set VS=%%i
if "%VS%"=="" set VS=%ProgramFiles(x86)%\Microsoft Visual Studio\18\BuildTools
call "%VS%\VC\Auxiliary\Build\vcvars64.bat" >nul || exit /b 1
cl /nologo /O2 /MT /EHsc /std:c++17 /LD %NAME%.cpp /Fe:%NAME%.dll /link /DLL
