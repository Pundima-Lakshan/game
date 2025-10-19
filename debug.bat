@echo off

cd "./build/"
cl -Zi "../src/main.cpp" user32.lib gdi32.lib
raddbg
cd "../"
