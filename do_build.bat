@echo off
cd /d c:\systeam123\system-client

if not exist build_new mkdir build_new
cd build_new

set PYTHON_PATH=
if exist "C:\client\python-embed\python.exe" set PYTHON_PATH=-DPython3_EXECUTABLE=C:/client/python-embed/python.exe
if exist "C:\Python312\python.exe" set PYTHON_PATH=-DPython3_EXECUTABLE=C:/Python312/python.exe
if exist "%LOCALAPPDATA%\Programs\Python\Python312\python.exe" set PYTHON_PATH=-DPython3_EXECUTABLE=%LOCALAPPDATA%\Programs\Python\Python312\python.exe

cmake .. -G "Visual Studio 18 2026" -A x64 %PYTHON_PATH%
if errorlevel 1 (
    echo CMake configure FAILED
    exit /b 1
)

cmake --build . --config Release --target game-client
if errorlevel 1 (
    echo Build FAILED
    exit /b 1
)

echo.
echo SUCCESS! Executable: c:\systeam123\system-client\build_new\Release\DDNet.exe
exit /b 0
