@echo OFF

set PROJECT_DIR=%1
set PROJECT_BRANCH=%2
set PLUGIN_BRANCH=%3
set CI_WORK_DIR=%4
set PROJECT_NAME=StoreUeSdkTest

echo.
echo ===========================
echo ### PREPARE TEST PROJECT ##

call %CI_WORK_DIR%\cicd\prepare-test-project.bat %PROJECT_DIR% %PROJECT_BRANCH% %PLUGIN_BRANCH% %CI_WORK_DIR%
if not %errorlevel%==0 goto onFinish

echo.
echo ==========================
echo ### BUILD TEST PROJECT ###

set AT_PATH="C:\EpicGames\UE_4.27\Engine\Binaries\DotNET\AutomationTool.exe"
set UPROJECT_PATH=%PROJECT_DIR%\%PROJECT_NAME%.uproject
set OUTPUT_ROOT_DIR=%CI_WORK_DIR%\Builds
set SETTINGS_INI_PATH=%PROJECT_DIR%\Config\DefaultEngine.ini

echo.
echo ### AUTOMATION_TOOL_PATH: %AT_PATH%
echo ### UPROJECT_PATH: %UPROJECT_PATH%
echo ### OUTPUT_ROOT_DIR: %OUTPUT_ROOT_DIR%
echo ### SETTINGS_INI_PATH: %SETTINGS_INI_PATH%

echo =====================================
echo ### BUILD IN GAME BROWSER VARIANT ###

call %CI_WORK_DIR%\cicd\change-ini-settings.bat %SETTINGS_INI_PATH% "/Script/XsollaSettings.XsollaProjectSettings" UsePlatformBrowser False
if not %errorlevel%==0 goto onFinish

call %AT_PATH% BuildCookRun -platform=Android -project=%UPROJECT_PATH% -archivedirectory=%OUTPUT_ROOT_DIR%\InGameBrowser -cookflavor=Multi -nop4 -cook -build -stage -prereqss -package -archive 
if not %errorlevel%==0 goto onFinish

echo ======================================
echo ### BUILD PLATFORM BROWSER VARIANT ###

call %CI_WORK_DIR%\cicd\change-ini-settings.bat %SETTINGS_INI_PATH% "/Script/XsollaSettings.XsollaProjectSettings" UsePlatformBrowser True
if not %errorlevel%==0 goto onFinish

call %AT_PATH% BuildCookRun -platform=Android -project=%UPROJECT_PATH% -archivedirectory=%OUTPUT_ROOT_DIR%\PlatformBrowser -cookflavor=Multi -nop4 -cook -build -stage -prereqss -package -archive 
if not %errorlevel%==0 goto onFinish

:onFinish
exit /b %errorlevel%