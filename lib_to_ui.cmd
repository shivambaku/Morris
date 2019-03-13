xcopy "%~dp0Morris\build\Release\*.lib" "%~dp0Morris_UI\cpp_addons\lib" /Y /s /e
xcopy "%~dp0Morris\build\x64\Release\*.lib" "%~dp0Morris_UI\cpp_addons\lib" /Y /s /e
xcopy "%~dp0Morris\src\*.h" "%~dp0Morris_UI\cpp_addons\inc\morris" /Y /s /e
xcopy "%~dp0Morris\src\*.hpp" "%~dp0Morris_UI\cpp_addons\inc\morris" /Y /s /e