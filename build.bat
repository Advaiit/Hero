@echo off
pushd build
cl -Zi ..\source\win32_hero.cpp user32.lib Gdi32.lib
popd
