@echo off
REM Check if a file was dragged onto the script
if "%~1"=="" (
    echo Bitte eine BMP-Datei auf dieses Script ziehen.
    pause
    exit /b
)

REM Set input file and output file
set "input=%~1"
set "filename=%~n1"
set "output=%~dp1%filename%.raw"

REM Call Python script
python bmp_to_raw_rgb.py "%input%" "%output%"

pause
