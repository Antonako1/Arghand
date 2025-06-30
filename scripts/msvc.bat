@echo off
REM Adds MSVC to the PATH if not already present.
REM Usage: msvc.bat [path_to_devcmd.bat]
cd /d %~dp0
set "DEVCMD=devcmd.bat"
if not "%~1"=="" (
    set "DEVCMD=%~1"
)
where cl.exe
if %errorlevel% neq 0 (
    echo cl.exe not found. Running "%DEVCMD%" to set up MSVC...
    call "%DEVCMD%"
    echo MSVC has been set up and added to PATH.
) else (
    echo MSVC already in PATH.
)
