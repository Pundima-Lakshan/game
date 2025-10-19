@echo off

cd "./build/"
cl "../src/main.cpp" user32.lib gdi32.lib
cd "../"
