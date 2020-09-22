:: Need add target/source parameters for usability
SET target=..\..\..\..\..\..\XsollaLogin
SET source=..\..\..\..\..

:: Remove/Create solution
rmdir %target% /S /Q
mkdir %target%"

:: Make full copy of useful for Project directories/files
for %%a in ("Source" "Content" "Config") do (xcopy /I /S "%source%\%%~a" "%target%\%%~a")
xcopy %source%\*.uproject %target%

:: Make full copy of useful for Plugin directories/files for Plugin
for %%a in ("Source" "Content" "Config" "Documentation" "Extras" "Resources") do (xcopy /I /S "%source%\Plugins\store-ue4-sdk\%%~a" "%target%\Plugins\store-ue4-sdk\%%~a")
xcopy "%source%\Plugins\store-ue4-sdk\*.uplugin" "%target%\Plugins\store-ue4-sdk\"

:: Remove Store directories/files
rmdir "%target%\Plugins\store-ue4-sdk\Content\Store" /S /Q
rmdir "%target%\Plugins\store-ue4-sdk\Source\XsollaStore" /S /Q

:: Need remove XsollaStore module from .uproject
rename %target%\Plugins\store-ue4-sdk login-ue4-sdk
:: Need rename in Xsolla.uplugin fields: FriendlyName, Description

:: Regenerate solution
@PAUSE