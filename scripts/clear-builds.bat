@echo off
setlocal
cd /d %~dp0
echo === Clearing previous builds ===
cd ..\Tests
echo Clearing Tests build directory...
if exist build (
    rmdir /s /q build
    echo Build directory cleared.
) else (
    echo No build directory found to clear.
)

echo === All build directories cleared ===
endlocal