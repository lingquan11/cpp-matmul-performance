@echo off
rem AI 脚手架：先验证 resident 算术与正确性，填 P 核掩码和结果目录。
setlocal
set "MASK="
set "EXE=%~1"
set "RESULT=%~2"
if not defined MASK (echo Fill affinity mask first. & exit /b 2)
if not exist "%EXE%" (echo Usage: run-planb.bat absolute-exe new-result-dir & exit /b 2)
if "%RESULT%"=="" exit /b 2
if exist "%RESULT%" (echo Results exist. & exit /b 2)
mkdir "%RESULT%" || exit /b 2
rem 顺序交替，独立进程；大于 256 的规模待接入动态 kernel。
for %%N in (64 256) do for %%R in (01 02 03 04 05 06 07 08 09 10) do (
  set /a parity=1%%R %% 2
  call :pair %%N %%R
  if errorlevel 1 exit /b 1
)
exit /b 0
:pair
if %parity%==0 (set "FIRST=real" & set "SECOND=resident") else (set "FIRST=resident" & set "SECOND=real")
call :one %1 %2 %FIRST% || exit /b 1
call :one %1 %2 %SECOND% || exit /b 1
exit /b 0
:one
mkdir "%RESULT%\n%1-run%2-%3" || exit /b 1
pushd "%RESULT%\n%1-run%2-%3" || exit /b 1
start "" /wait /affinity %MASK% /high "%EXE%" ikj %1 5 --variant %3 --out sample.csv
set "CODE=%errorlevel%"
popd
if not "%CODE%"=="0" exit /b %CODE%
timeout /t 120 /nobreak >nul
exit /b 0
