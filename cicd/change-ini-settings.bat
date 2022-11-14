@echo OFF

set SETTINGS_INI_PATH=%1
set VARIABLE_CATEGORY=%2
set VARIABLE_NAME=%3
set VARIABLE_VALUE=%4
set INI_TOOL_PATH="C:\Users\Runner\initool\initool.exe"
set TEMP_INI_PATH="C:\Users\Runner\initool\temp.ini"

echo.
echo ===========================
echo ### CHANGE INI SETTINGS ###
echo ### SETTINGS_INI_PATH: %SETTINGS_INI_PATH%
echo ### VARIABLE_CATEGORY: %VARIABLE_CATEGORY%
echo ### VARIABLE_NAME: %VARIABLE_NAME%
echo ### VARIABLE_VALUE: %VARIABLE_VALUE%
echo ### INI_TOOL_PATH: %INI_TOOL_PATH%
echo ### TEMP_INI_PATH: %TEMP_INI_PATH%
echo.

call %INI_TOOL_PATH% s %SETTINGS_INI_PATH% %VARIABLE_CATEGORY% %VARIABLE_NAME% %VARIABLE_VALUE% > %TEMP_INI_PATH%
if not %errorlevel%==0 goto onFinish

move /y %TEMP_INI_PATH% %SETTINGS_INI_PATH%
if not %errorlevel%==0 goto onFinish

:onFinish
exit /b %errorlevel%