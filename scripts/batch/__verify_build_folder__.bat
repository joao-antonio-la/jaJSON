@echo off
REM Check if we are in the project root (README.md, src, include must exist)
if not exist "README.md" (
    echo This script was created to be called from the project root dir
    exit /b 1
)
if not exist "src\" (
    echo This script was created to be called from the project root dir
    exit /b 1
)
if not exist "include\" (
    echo This script was created to be called from the project root dir
    exit /b 1
)

if not exist "build\" (
    echo Creating directory build
    mkdir "build"
)

if not exist "build\tests\" (
    echo Creating directory build\tests
    mkdir "build\tests"
)

if not exist "build\examples\" (
    echo Creating directory build\examples
    mkdir "build\examples"
)

if not exist "build\out\" (
    echo Creating directory build\out
    mkdir "build\out"
)

if not exist "build\data" (
    echo Creating directory build\data
    mkdir "build\data"
)

echo Build folder structure verified.
exit /b 0
