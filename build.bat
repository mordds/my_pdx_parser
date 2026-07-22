@echo off
cd build
if "%~1"=="" (
    cmake .. -DCMAKE_CXX_COMPILER="D:\msys2\mingw64\bin\g++.exe" -DCMAKE_C_COMPILER="D:\msys2\mingw64\bin\gcc.exe" -G "MSYS Makefiles"
    make console
    cp console.exe ../console.exe
) else (
    cmake .. -DCMAKE_CXX_COMPILER="D:\msys2\mingw64\bin\g++.exe" -DCMAKE_C_COMPILER="D:\msys2\mingw64\bin\gcc.exe" -G "MSYS Makefiles" 
    make %1
    if not "%1"=="clean" cp %1.exe ../%1.exe
)

