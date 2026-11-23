@echo off
rem AI 脚手架。先填 P 核掩码并实现 kernel；不得覆盖结果。参见报告与 metadata。
setlocal
set "AFFINITY_MASK="
set "COOLDOWN_SEC=120"
set "TIMEOUT_SEC=15"
set "EXE=%~1"
set "RESULT=%~2"
if not defined AFFINITY_MASK (echo Fill AFFINITY_MASK after core scan. & exit /b 2)
if not exist "%EXE%" (echo Usage: run-scale-sweep.bat absolute-exe absolute-new-results-dir & exit /b 2)
if "%RESULT%"=="" exit /b 2
if exist "%RESULT%" (echo Result directory already exists. & exit /b 2)
mkdir "%RESULT%" || exit /b 2
for %%R in (01 02 03) do for %%N in (256 384 512 768 1024 1536) do for %%A in (ikj blocked_64 blocked_128) do (
  mkdir "%RESULT%\run%%R-%%N-%%A" || exit /b 1
  powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0scale-sweep-launch.ps1" -Exe "%EXE%" -Directory "%RESULT%\run%%R-%%N-%%A" -Mask "%AFFINITY_MASK%" -N %%N -Algorithm %%A -TimeoutSec %TIMEOUT_SEC%
  if errorlevel 1 exit /b 1
  timeout /t %COOLDOWN_SEC% /nobreak >nul
)
