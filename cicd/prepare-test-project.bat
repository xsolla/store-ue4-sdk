@echo OFF

set PROJECT_DIR=%1
set PROJECT_BRANCH=%2
set PLUGIN_BRANCH=%3
set CI_WORK_DIR=%4
set ENGINE_VERSION=%5%

echo.
echo ==========================
echo ### CLONE TEST PROJECT ###

set PROJECT_REMOTE=git@gitlab.loc:sdk_group/store-ue-sdk-test.git

echo ### PROJECT_REMOTE: %PROJECT_REMOTE%
echo ### PROJECT_DIR: %PROJECT_DIR%
echo ### PROJECT_BRANCH: %PROJECT_BRANCH%

if exist %PROJECT_DIR% rd /q /s %PROJECT_DIR%
if not %errorlevel%==0 goto onFinish

git clone --depth 1 --branch %PROJECT_BRANCH% %PROJECT_REMOTE% %PROJECT_DIR%
if not %errorlevel%==0 goto onFinish

echo.
echo ===============================
echo ### CLONE PLUGIN TO PROJECT ###

set PLUGIN_REMOTE=git@gitlab.loc:sdk_group/store-ue4-sdk.git
set PLUGIN_DIR=%PROJECT_DIR%\Plugins\XsollaSdk

echo ### PLUGIN_REMOTE: %PLUGIN_REMOTE%
echo ### PLUGIN_DIR: %PLUGIN_DIR%
echo ### PLUGIN_BRANCH: %PLUGIN_BRANCH%

git clone --depth 1 --branch %PLUGIN_BRANCH% %PLUGIN_REMOTE% %PLUGIN_DIR%
if not %errorlevel%==0 goto onFinish

echo.
echo ========================================
echo ### SWITCH ENGINE VERSION FOR PROJECT ##

set UVS_PATH="C:\Program Files (x86)\Epic Games\Launcher\Engine\Binaries\Win64\UnrealVersionSelector.exe"
set UPROJECT_PATH="%PROJECT_DIR%\%PROJECT_NAME%.uproject"
set ENGINE_ROOT_PATH="C:\EpicGames\UE_%ENGINE_VERSION%"

echo ### VERSION_SELECTOR_PATH: %UVS_PATH%
echo ### UPROJECT_PATH: %UPROJECT_PATH%
echo ### ENGINE_ROOT_PATH: %ENGINE_ROOT_PATH%

call %UVS_PATH% /switchversionsilent %UPROJECT_PATH% %ENGINE_ROOT_PATH%
if not %errorlevel%==0 goto onFinish

echo.
echo ========================================
echo ### SWITCH ENGINE VERSION FOR PLUGIN ##

set PLUGIN_CONFIG_PATH=%PLUGIN_DIR%\Xsolla.uplugin

echo ### PLUGIN_CONFIG_PATH: %PLUGIN_CONFIG_PATH%

python -c "import json; data = json.load(open(r'%PLUGIN_CONFIG_PATH%')); data['EngineVersion'] = '%ENGINE_VERSION%'; json.dump(data, open(r'%PLUGIN_CONFIG_PATH%', 'w'), indent=2)"
if not %errorlevel%==0 goto onFinish

rem ========================================
:onFinish
set EXIT_CODE=%errorlevel%
cd %CI_WORK_DIR%
exit /b %EXIT_CODE%