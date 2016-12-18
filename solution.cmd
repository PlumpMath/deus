@echo off
pushd %~dp0

rem Project
for /f "tokens=2 delims=( " %%i in ('findstr /c:"project(" CMakeLists.txt') do (
  set project=%%i
)

rem Settings
set generator=Visual Studio 15 2017 Win64
set toolset=v141
set configs=Release;Debug

call third_party\settings.cmd

echo.
echo Project        : "%project%"
echo Generator      : "%generator%"
echo Toolset        : "%toolset%"
echo Configurations : "%configs%"
echo.

rem Build Directory
if not exist build (
  mkdir build
)
pushd build

if not exist msvc (
  mkdir msvc
)
pushd msvc

rem Solution
cmake -G "%generator%" -T "%toolset%" -DCMAKE_CONFIGURATION_TYPES="%configs%" -DCMAKE_BINARY_DIR:PATH=. -DCMAKE_INSTALL_PREFIX:PATH=../.. ../..
if %errorlevel% == 0 (
  start %project%.sln
) else (
  pause
)

popd
popd
popd