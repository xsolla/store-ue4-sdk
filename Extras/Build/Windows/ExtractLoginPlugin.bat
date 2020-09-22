SET source=..\..\..\..\..\..\XsollaLogin\Plugins\login-ue4-sdk
SET target=..\..\..\..\..\..\ue4-login-sdk

rmdir %target% /S /Q
mkdir %target%

:: Folders
for %%a in ("Source" "Content" "Config" "Documentation" "Extras" "Resources") do (xcopy /I /S "%source%\%%~a" "%target%\%%~a")

:: Files
xcopy "%source%\*.uplugin" "%target%"

@PAUSE