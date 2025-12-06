@echo off
REM Usage: run_test.bat <source_file.c>

REM === CHECKS USAGE ===
if "%~1"=="" (
    echo Usage: %~nx0 ^<source_file.c^>
    exit /b 1
)

REM === VERIFING FOLDER STRUCTURE ===
set __verify_build_folder__=.\scripts\batch\__verify_build_folder__.bat
if exist "%__verify_build_folder__%" (
    call "%verify_build%"
) else (
    echo Error: %__verify_build_folder__% not found!
    exit /b 1
)

REM === CONFIGURATION ===
set SRC=%~1
set BASENAME=%~n1
set OUT=build\out\%BASENAME%.out

REM === COMPILING ===
echo Compiling %SRC% -^> %OUT%
gcc-Iinclude -Isrc src\jajson.c "%SRC%" -o "%OUT%" -lm -D__USE_MINGW_ANSI_STDIO

REM === EXECUTING ===
if %errorlevel% equ 0 (
    "%OUT%"
) else (
    echo Compilation failed.
)