@echo off

REM Ask the user for a project name.
set /p project=Enter the project name:
echo Project name is %project%

REM Rename `Template.sln`, `Tempalte.vcxproj`, `Template.vcxproj.filters`, and `Template.vcxproj.user` to the project name.
ren Template.sln %project%.sln
ren Template.vcxproj %project%.vcxproj
ren Template.vcxproj.filters %project%.vcxproj.filters
ren Template.vcxproj.user %project%.vcxproj.user

REM Replace all instances of `Template` with the project name in the solution file.
setlocal enabledelayedexpansion
set "search=Template"
set "replace=!project!"
set "tempfile=%project%.sln.tmp"
del /q "%tempfile%" 2>nul

for /f "delims=" %%i in (%project%.sln) do (
    set "line=%%i"
    set "line=!line:%search%=%replace%!"
    echo !line!>>"%tempfile%"
)

REM Delete the original solution file and rename the temporary file.
del %project%.sln
ren %tempfile% %project%.sln

REM Replace all instances of `Template` with the project name in the project file.
set "tempfile=%project%.vcxproj.tmp"
del /q "%tempfile%" 2>nul

for /f "delims=" %%i in (%project%.vcxproj) do (
    set "line=%%i"
    set "line=!line:%search%=%replace%!"
    echo !line!>>"%tempfile%"
)

REM Delete the original project file and rename the temporary file.
del %project%.vcxproj
ren %tempfile% %project%.vcxproj

REM Pause the script so the user can see the output.
pause
