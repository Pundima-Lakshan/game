@echo off

cd "./build/"
cl -Zi "../src/main.c"
raddbg
cd "../"
