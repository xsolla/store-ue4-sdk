@echo OFF

set PROJECT_DIR=%1
set PROJECT_BRANCH=%2
set PLUGIN_BRANCH=%3
set CI_WORK_DIR=%4

echo ==========================
echo ### CLONE TEST PROJECT ###
set PROJECT_REMOTE=git@gitlab.loc:sdk_group/store-ue-sdk-test.git

echo ### PROJECT_REMOTE: %PROJECT_REMOTE%
echo ### PROJECT_DIR: %PROJECT_DIR%
echo ### PROJECT_BRANCH: %PROJECT_BRANCH%
echo.

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
echo.

git clone --depth 1 --branch %PLUGIN_BRANCH% %PLUGIN_REMOTE% %PLUGIN_DIR%
if not %errorlevel%==0 goto onFinish

:onFinish
set EXIT_CODE=%errorlevel%
cd %CI_WORK_DIR%
exit /b %EXIT_CODE%