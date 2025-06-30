@echo off
setlocal
cd /d %~dp0
cd ..\Tests
if not exist build mkdir build
cd build
cmake ..
cmake --build . --config Debug
call .\arghand-tests.exe %*
echo %ERRORLEVEL%