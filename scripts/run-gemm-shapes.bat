@echo off
rem AI 维护：正式测量前填写已核实的 P 核掩码，先实现和验证内核。
setlocal
set "MASK="
set "EXE=%~1"
set "RESULT=%~2"
if not defined MASK (echo Fill affinity mask first. & exit /b 2)
if not exist "%EXE%" (echo Usage: run-gemm-shapes.bat absolute-exe new-results-directory & exit /b 2)
if "%RESULT%"=="" exit /b 2
if exist "%RESULT%" (echo Result directory exists. & exit /b 2)
mkdir "%RESULT%" || exit /b 2
for %%R in (01 02 03 04 05 06 07 08 09 10) do for %%S in (square tall wide deep shortk) do (
  mkdir "%RESULT%\run%%R-%%S" || exit /b 1
  pushd "%RESULT%\run%%R-%%S" || exit /b 1
  start "" /wait /affinity %MASK% /high "%EXE%" --shape %%S --runs 5 --out sample.csv
  if errorlevel 1 (popd & exit /b 1)
  popd
  timeout /t 120 /nobreak >nul
)
