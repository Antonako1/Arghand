@echo off
REM Usage: create-package.bat {dev|prod}
REM dev : Development build, includes sources, tests, headers and binaries
REM prod: Production build, includes only binaries

setlocal

set "BUILD_TYPE=%1"
if "%BUILD_TYPE%"=="" (
    echo Usage: create-package.bat {dev|prod}
    exit /b 1
)



endlocal