@echo off
rem AI 脚手架：填入 C1 绑核掩码和结果目录；先实现并测试核心函数。
setlocal
set "MASK="
set "COOLDOWN=120"
set "EXE=%~1"
set "RESULT=%~2"
if not defined MASK (echo Fill C1 affinity mask. & exit /b 2)
if not exist "%EXE%" (echo Usage: run-packed-block.bat absolute-exe new-absolute-results-dir & exit /b 2)
if "%RESULT%"=="" exit /b 2
if exist "%RESULT%" (echo Result directory exists. & exit /b 2)
mkdir "%RESULT%" || exit /b 2
for %%N in (128 256) do for %%B in (32 64) do for %%R in (01 02 03 04 05 06 07 08 09 10) do for %%A in (ikj blocked blocked_packed pack_only) do (
  mkdir "%RESULT%\n%%N-b%%B-run%%R-%%A" || exit /b 1
  pushd "%RESULT%\n%%N-b%%B-run%%R-%%A" || exit /b 1
  start "" /wait /affinity %MASK% /high "%EXE%" --algo %%A --n %%N --b %%B --runs 5 --out sample.csv
  if errorlevel 1 (popd & exit /b 1)
  popd
  timeout /t %COOLDOWN% /nobreak >nul
)
