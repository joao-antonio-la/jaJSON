@echo off
setlocal enabledelayedexpansion
REM Usage: run_test.bat <test_file_name.c>

REM === CHECKS USAGE ===
if "%~1"=="" (
    echo Usage: %~nx0 ^<test_file_name.c^>
    exit /b 1
)

REM === VERIFING FOLDER STRUCTURE ===
set __verify_build_folder__=.\scripts\batch\__verify_build_folder__.bat
if exist "%__verify_build_folder__%" (
    call "%__verify_build_folder__%"
) else (
    echo Error: %__verify_build_folder__% not found!
    exit /b 1
)

REM === CONFIGURATION ===
set SRC=%~1
set BASENAME=%~n1
set OUT=build\tests\%BASENAME%.out
set OK=[32m OK[0m
set FAIL=[31m FAIL[0m

REM === COMPILING ===
echo Compiling %SRC% -^> %OUT%
gcc -Wall -Wextra -Iinclude -Isrc src\jajson.c "%SRC%" -o "%OUT%" -lm -D__USE_MINGW_ANSI_STDIO

REM === EXECUTING ===
if !errorlevel! equ 0 (
    "%OUT%"
    if !errorlevel! equ 0 (
        echo Test batch completed successfully. %OK%
    ) else (
        echo Test batch failed. %FAIL%
    )
) else (
    echo Compilation failed.
)
