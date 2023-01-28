@echo off

pushd ..\build

cl -Od -D_DEBUG -nologo /GS- /Gs999999 ../code/win32_templates.cpp /link /nodefaultlib /subsystem:windows user32.lib kernel32.lib gdi32.lib

popd
