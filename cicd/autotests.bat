@echo OFF

set PROJECT_DIR=%1
set PROJECT_BRANCH=%2
set PLUGIN_BRANCH=%3
set CI_WORK_DIR=%4
set ENGINE_VERSION=%5
set PROJECT_NAME=StoreUeSdkTest

echo.
echo ============================
echo ### PREPARE TEST PROJECT ###

call %CI_WORK_DIR%\cicd\prepare-test-project.bat %PROJECT_DIR% %PROJECT_BRANCH% %PLUGIN_BRANCH% %CI_WORK_DIR% %ENGINE_VERSION%
if not %errorlevel%==0 goto onFinish

echo.
echo =============================
echo ### GENERATE TEST PROJECT ###

set UPROJECT_PATH=%PROJECT_DIR%\%PROJECT_NAME%.uproject
set UBT_PATH="C:\EpicGames\UE_4.27\Engine\Binaries\DotNET\UnrealBuildTool.exe"

echo ### UPROJECT_PATH: %UPROJECT_PATH%
echo ### UBT_PATH: %UBT_PATH%

call %UBT_PATH% -Mode=QueryTargets -Project=%UPROJECT_PATH%
if not %errorlevel%==0 goto onFinish

call %UBT_PATH% Development Win64 -TargetType=Editor -Project=%UPROJECT_PATH% -Progress -NoEngineChanges -NoHotReloadFromIDE
if not %errorlevel%==0 goto onFinish

echo.
echo ====================
echo ### LAUNCH TESTS ###

set UE_PATH="C:\EpicGames\UE_4.27\Engine\Binaries\Win64\UE4Editor.exe"
set REPORTS_DIR=%CI_WORK_DIR%\Logs

echo ### UPROJECT_PATH: %UPROJECT_PATH%
echo ### UE_PATH: %UE_PATH%
echo ### REPORTS_DIR: %REPORTS_DIR%
echo.

call %UE_PATH% %UPROJECT_PATH% -ExecCmds="Automation RunTests Xsolla" -ReportOutputPath=%REPORTS_DIR% -nullRHI -nopause -unattended -testexit="Automation Test Queue Empty"
set errorlevel=0

echo.
echo ===========================
echo ### INSPECT TESTS REPORT ###

set JSON_PARSER_PATH=C:\Users\Runner\jq-win64.exe
set REPORT_PATH=%REPORTS_DIR%\index.json

echo ### JSON_PARSER_PATH: %JSON_PARSER_PATH%
echo ### REPORT_PATH: %REPORT_PATH%

FOR /F %%i IN ('type %REPORT_PATH% ^| "%JSON_PARSER_PATH%" ".succeeded"') DO set SUCCEEDED_TESTS=%%i
if not %errorlevel%==0 goto onFinish

FOR /F %%i IN ('type %REPORT_PATH% ^| "%JSON_PARSER_PATH%" ".failed"') DO set FAILED_TESTS=%%i
if not %errorlevel%==0 goto onFinish

echo ### SUCCEEDED TESTS: %SUCCEEDED_TESTS%
echo ### FAILED TESTS: %FAILED_TESTS%
if %FAILED_TESTS% NEQ 0 ( 
	set errorlevel=1
)

:onFinish
exit /b %errorlevel%