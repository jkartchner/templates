@echo off

pushd ..\build

cl -Zi -Od -D_DEBUG -nologo -Fetemplates_debug.exe /GS- /Gs999999 ../code/win32_templates.cpp /link /nodefaultlib /subsystem:windows user32.lib kernel32.lib gdi32.lib

cl -O2 -nologo /GS- /Gs999999 -Fetemplates_release.exe ../code/win32_templates.cpp /link /nodefaultlib /subsystem:windows user32.lib kernel32.lib gdi32.lib

popd
