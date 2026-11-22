@echo off
rem AI 脚手架：仅供预测已 commit、正确性通过、测量地基就绪后使用。
setlocal
set "AFFINITY_MASK="
set "N=1536"
set "COOLDOWN_SEC=120"
set "TIMEOUT_SEC=15"
set "EXE=%~1"
set "RESULT=%~2"
if not defined AFFINITY_MASK (echo Fill P-core affinity mask first. & exit /b 2)
if not exist "%EXE%" (echo Usage: run-block-sweep.bat absolute-exe new-absolute-result-dir & exit /b 2)
if "%RESULT%"=="" exit /b 2
if exist "%RESULT%" (echo Result directory already exists. & exit /b 2)
mkdir "%RESULT%" || exit /b 2
for %%R in (01 02 03) do for %%B in (0 16 32 64 128 256) do (
  mkdir "%RESULT%\run%%R-block%%B" || exit /b 1
  if %%B==0 (set "ALGO=ikj") else (set "ALGO=blocked")
  call :run %%R %%B
  if errorlevel 1 exit /b 1
  timeout /t %COOLDOWN_SEC% /nobreak >nul
)
exit /b 0
:run
powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0scale-sweep-launch.ps1" -Exe "%EXE%" -Directory "%RESULT%\run%1-block%2" -Mask "%AFFINITY_MASK%" -N %N% -Algorithm "%ALGO%" -TimeoutSec %TIMEOUT_SEC% -Block %2
exit /b %errorlevel%
