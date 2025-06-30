@echo off
rem Create distribution package for Arghand

setlocal enabledelayedexpansion

rem Move to script directory
cd /d %~dp0
cd ..

rem Read version string from file
set /p _version=<Arghand\VERSION

rem Clean version string (optional if needed)
set _version=%_version: =%
set _package=Arghand-%_version%

rem Create package directory
mkdir "%_package%"
mkdir "%_package%\Include"

rem Copy files
copy LICENSE "%_package%\LICENSE.txt"
copy README.md "%_package%\README.md"
copy Arghand\Include\*.h "%_package%\Include\"

echo.
echo Created package: %_package%
echo Location: %cd%\%_package%

rem Create .7z archive
if not exist "C:\Program Files\7-Zip\7z.exe" (
    echo 7-Zip is not installed. Please install it from https://www.7-zip.org/
    exit /b 1
)
if not exist "%_package%" (
    echo Package directory does not exist. Please check the previous steps.
    exit /b 1
)
"C:\Program Files\7-Zip\7z.exe" a -t7z "%_package%.7z" "%_package%\*"
if %ERRORLEVEL% neq 0 (
    echo Failed to create archive. Please check the 7-Zip installation and try again.
    exit /b 1
)
echo.
echo Created archive: %_package%.7z
echo Location: %cd%\%_package%.7z

endlocal
