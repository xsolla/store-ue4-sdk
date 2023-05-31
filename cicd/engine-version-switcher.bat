@echo OFF

set PROJECT_DIR=%1
set UE_VERSION=%2
set PROJECT_NAME=StoreUeSdkTest

echo.
echo ========================================
echo ### SWITCH ENGINE VERSION FOR PROJECT ##

set UVS_PATH="C:\Program Files (x86)\Epic Games\Launcher\Engine\Binaries\Win64\UnrealVersionSelector.exe"
set UPROJECT_PATH="%PROJECT_DIR%\%PROJECT_NAME%.uproject"
set ENGINE_ROOT_PATH="C:\EpicGames\UE_%UE_VERSION%"

echo.
echo ### VERSION_SELECTOR_PATH: %UVS_PATH%
echo ### UPROJECT_PATH: %UPROJECT_PATH%
echo ### ENGINE_ROOT_PATH: %ENGINE_ROOT_PATH%

call %UVS_PATH% /switchversionsilent %UPROJECT_PATH% %ENGINE_ROOT_PATH%
if not %errorlevel%==0 goto onFinish

:onFinish
exit /b %errorlevel%