@echo off
PATH C:\MinGW\bin\
g++ Main.cpp -o Main.exe -static-libgcc -static-libstdc++ -lwinmm
pause