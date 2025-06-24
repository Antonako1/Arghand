@echo off
setlocal
cd /d %~dp0
call .\make-lib.bat Debug
xcopy /E /Y ..\Arghand\install\ ..\Tests\install\
cd ..\Tests
if not exist build mkdir build
cd build
cmake ..
cmake --build . --config Debug
copy /B /Y ..\install\bin\Arghand.dll .\Arghand.dll
call .\arghand-tests.exe
echo %ERRORLEVEL%