@echo off
:fak
make
pause
if %ERRORLEVEL% GEQ 1 goto fak
C:\Users\Jasper\Documents\EmuCR-Citra-20150510-x64\citra-qt.exe othello.3dsx
goto fak