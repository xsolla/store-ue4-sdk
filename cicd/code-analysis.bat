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
echo =============================
echo ### GENERATE TEST PROJECT ###

set UPROJECT_PATH=%PROJECT_DIR%\%PROJECT_NAME%.uproject
set UAT_PATH="C:\EpicGames\UE_4.27\Engine\Binaries\DotNET\AutomationTool.exe"
set UVS_PATH="C:\Program Files (x86)\Epic Games\Launcher\Engine\Binaries\Win64\UnrealVersionSelector.exe"

echo ### UPROJECT_PATH: %UPROJECT_PATH%
echo ### AUTOMATION_TOOL_PATH: %UAT_PATH%
echo ### UNIVERSAL_VERSION_SELECTOR_PATH: %UVS_PATH%
echo.

call %UAT_PATH% BuildCookRun -platform=Win64 -project=%UPROJECT_PATH% -nop4 -cook -build -stage -prereqs
if not %errorlevel%==0 goto onFinish

call %UVS_PATH% /projectfiles %UPROJECT_PATH%
if not %errorlevel%==0 goto onFinish

echo.
echo ============================
echo ### LAUNCH CODE ANALYZER ###

set USLN_PATH=%PROJECT_DIR%\%PROJECT_NAME%.sln
set REPORT_PATH=%CI_WORK_DIR%\Logs\code-analysis-report.xml

echo ### UPROJECT_PATH: %UPROJECT_PATH%
echo ### USLN_PATH: %USLN_PATH%
echo ### REPORT_PATH: %REPORT_PATH%
echo.

set CODE_ANALYZER_PATH=C:\JetBrains.ReSharper.CommandLineTools.2021.1.3\inspectcode.exe
call %CODE_ANALYZER_PATH% "%USLN_PATH%" --project=%PROJECT_NAME% -o="%REPORT_PATH%" -s=ERROR --exclude="Plugins\XsollaSdkTests\**.*"
if not %errorlevel%==0 goto onFinish

echo.
echo ====================================
echo ### INSPECT CODE ANALYSIS REPORT ###

set XML_INSPECTOR_PATH=C:\xmlstarlet-1.6.1-win32\xmlstarlet-1.6.1\xml.exe
FOR /F %%i IN ('%XML_INSPECTOR_PATH% sel -t -v "count(/Report/IssueTypes/IssueType)" %REPORT_PATH%') DO set ISSUES=%%i
if not %errorlevel%==0 goto onFinish

if %ISSUES% NEQ 0 ( 
	set errorlevel=1
)

:onFinish
exit /b %errorlevel%