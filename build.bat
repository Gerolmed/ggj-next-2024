@echo off
del game.exe >nul
cd build
msbuild /nologo /v:q game.sln
move Debug\game.exe ..\game.exe >nul
