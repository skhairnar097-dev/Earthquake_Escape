@echo off
title Earthquake Escape - 3D Simulation Game
echo ========================================================
echo   EARTHQUAKE ESCAPE: 3D COMPUTER GRAPHICS PROJECT
echo ========================================================
echo.
echo Starting local web server on http://localhost:8000 ...
echo Opening Earthquake Escape in your default browser...
echo.

start http://localhost:8000
python -m http.server 8000
pause
