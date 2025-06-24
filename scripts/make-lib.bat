@echo off
cd /d %~dp0
setlocal

:: Build modes: Debug, Release
SET "BUILD_MODE=Release"
if "%1" NEQ "" (
    SET "BUILD_MODE=%1"
)

echo === Building Arghand Library ===

REM Navigate to the Arghand project directory
cd ..\Arghand

REM Create build directory if it doesn't exist
if not exist build (
    mkdir build
)

REM Enter build directory
cd build

REM Run CMake configuration
cmake -DCMAKE_INSTALL_PREFIX=../install -DCMAKE_BUILD_TYPE=%BUILD_MODE% ..
if errorlevel 1 goto BuildFailed

REM Build and install the library
cmake --build . --target install
if errorlevel 1 goto BuildFailed

echo === Build and install successful ===
goto End

:BuildFailed
echo Build failed with error code %ERRORLEVEL%.
exit /b %ERRORLEVEL%

:End
endlocal
exit /b 0
