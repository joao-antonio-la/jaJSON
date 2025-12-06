@echo off
setlocal enabledelayedexpansion

echo ==========================================
echo        jaJSON - Test Suite Runner
echo ==========================================
echo.

REM === CONFIGURATION ===
set TEST_DIR=tests\src
set SCRIPT_DIR=scripts\batch
set RUN_TEST=%SCRIPT_DIR%\run_test.bat
set STOP_ON_FAIL=0   REM 1 = stop after first failure, 0 = continue all
set PASSED=0
set FAILED=0

REM === VERIFY run_test.bat EXISTS ===
if not exist "%RUN_TEST%" (
    echo Error: "%RUN_TEST%" not found!
    exit /b 1
)

REM === FIND ALL test_*.c FILES ===
for %%F in ("%TEST_DIR%\test_*.c") do (
    echo Running test: %%~nxF
    call "%RUN_TEST%" "%%F"
    if !errorlevel! equ 0 (
        set /a PASSED+=1
    ) else (
        set /a FAILED+=1
        if !STOP_ON_FAIL! equ 1 (
            echo.
            echo Test failed. Stopping early due to STOP_ON_FAIL=1.
            goto :summary
        )
    )
    echo ------------------------------------------
)

:summary
echo.
echo ==========================================
echo              Test Summary
echo ==========================================
echo [32mPassed: !PASSED![0m
echo [31mFailed: !FAILED![0m
echo ==========================================
echo.

if !FAILED! gtr 0 (
    exit /b 1
) else (
    exit /b 0
)
