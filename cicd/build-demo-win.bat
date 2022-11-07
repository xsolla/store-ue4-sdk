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
set OUTPUT_DIR=%CI_WORK_DIR%\Builds\Win64

echo.
echo ### AUTOMATION_TOOL_PATH: %AT_PATH%
echo ### UPROJECT_PATH: %UPROJECT_PATH%
echo ### OUTPUT_DIR: %OUTPUT_DIR%

call %AT_PATH% BuildCookRun -platform=Win64 -project=%UPROJECT_PATH% -nop4 -cook -build -stage -prereqss -package -archive -archivedirectory=%OUTPUT_DIR%
if not %errorlevel%==0 goto onFinish

:onFinish
exit /b %errorlevel%