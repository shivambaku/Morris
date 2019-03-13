(npm -g list electron-packager || ^
npm install -g electron-packager) && ^
electron-packager . --asar --out="../ML UI Executable/" && ^
xcopy "%~dp0cpp_addons\lib\*.dll" "%~dp0\..\ML UI Executable\ML-UI-win32-x64" /Y