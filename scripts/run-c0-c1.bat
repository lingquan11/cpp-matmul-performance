@echo off
rem 由零醛根据核扫描结果填写。十六进制，不带 0x。不要把耗时填在这里。
rem 没填之前保持 TODO。这四个字母是 ASCII，避免中文 Windows 的代码页把占位符拆开。
rem 脚本会拒绝启动 exe，也不会猜一个 P 核。
set "P_CORE_MASK=TODO"

setlocal EnableExtensions
rem 这是什么：C0（不绑核）和 C1（绑一个 P 核，并加 /high）交错各跑 10 次。
rem 谁写的：脚手架由 AI 维护。P 核掩码和规则阈值都由零醛定。
rem 怎么运行：
rem   scripts\run-c0-c1.bat ^<day07.exe路径^> ^<输出根目录^> [dry-run]
rem   dry-run 只打印将要使用的目录和命令，不建目录，不启动 exe。
rem 限制：
rem   day07 没有参数，CSV 写到当前目录的 matmul_benchmark_day7_blocking.csv。
rem   每次先 mkdir，再 cd 进 c0\runNN\ 或 c1\runNN\，然后才 start exe。
rem   C0 不加 /affinity，也不加 /high。C1 用脚本顶部的 P_CORE_MASK，并加 /high。
rem   交错顺序：run01 的 C0、run01 的 C1、run02 的 C0、run02 的 C1，直到 run10。
rem   掩码写法与 run-core-scan.bat 相同：十六进制，不带 0x。
rem   调用形式：start "" /wait /high /affinity ^<P_CORE_MASK^> "^<exe^>"。
rem   C0/C1 合计超过 5 小时就停。已经写出的 runNN 留着，在 metadata 里说明停在第几轮。
rem   云端、Linux 或虚拟机里的毫秒不能写进正式 CSV 或报告。
rem   路径里不要带 & | < > ^ 这些符号。

chcp 65001 >nul 2>&1
ver >nul

if "%~1"=="" goto :usage
if "%~2"=="" goto :usage
if "%~3"=="" goto :args_ok
if /I "%~3"=="dry-run" goto :args_ok
echo REFUSE: the third argument must be dry-run, or omitted.
echo 第三个参数只能是 dry-run，或者不写。
exit /b 2
:args_ok

set "EXE=%~f1"
set "OUTROOT=%~f2"
set "MODE=%~3"
if "%OUTROOT:~-1%"=="\" set "OUTROOT=%OUTROOT:~0,-1%"

call :guard "%OUTROOT%"
if errorlevel 1 exit /b 2

call :checkmask
set "MASK_RC=%ERRORLEVEL%"

if /I "%MODE%"=="dry-run" (
  echo DRY-RUN: do not create directories and do not start the exe.
  echo dry-run：不创建目录，不启动 exe。
  call :print_plan
  if not "%MASK_RC%"=="0" (
    echo REFUSE: P_CORE_MASK is not ready. The lines above are a plan; start was not executed.
    echo 真实运行会拒绝：P_CORE_MASK 还不能用。上面只是目录计划，start 没有执行。
    exit /b 2
  )
  echo dry-run 结束。上面的 start 没有执行。
  exit /b 0
)

if not "%MASK_RC%"=="0" exit /b 2
if not exist "%EXE%" (
  echo 找不到 exe："%EXE%"
  exit /b 2
)
if exist "%CD%\matmul_benchmark_day7_blocking.csv" (
  echo 拒绝：启动目录里已经有 matmul_benchmark_day7_blocking.csv。
  echo 请换到没有这份 CSV 的目录再运行。
  exit /b 2
)

for /L %%R in (1,1,10) do (
  call :run_one c0 %%R nobind
  if errorlevel 1 exit /b 1
  call :run_one c1 %%R bind
  if errorlevel 1 exit /b 1
)

echo C0 和 C1 各 10 次都跑完了。数字只以各自 runNN 目录里的 CSV 为准。
exit /b 0

:usage
echo 用法: %~nx0 ^<day07.exe路径^> ^<输出根目录^> [dry-run]
echo 先在脚本顶部把 P_CORE_MASK 改成核扫描选定的 P 核掩码，十六进制，不带 0x。
echo 目录是 ^<输出根^>\c0\run01 到 run10，以及 ^<输出根^>\c1\run01 到 run10。
exit /b 2

:guard
setlocal
set "ROOT=%~1"
set "NORM=%ROOT:\=/%"
for %%I in ("%ROOT%") do set "LEAF=%%~nxI"
if exist "%ROOT%\src\day07-matrix-blocking.cpp" (
  echo REFUSE: output root is the repo root. Use a separate result directory.
  echo 拒绝：输出根是仓库根。请换成独立的结果目录。
  endlocal & exit /b 1
)
if /I "%LEAF%"=="results" (
  echo REFUSE: do not use experiments\results itself as the output root.
  echo 拒绝：不要把 experiments\results 本身当输出根。
  endlocal & exit /b 1
)
if /I "%LEAF%"=="YYYY-MM-DD-measure-baseline-windows" (
  echo REFUSE: rename the YYYY-MM-DD template before running. Do not write CSV into the placeholder.
  echo 拒绝：模板目录还没改成真实日期。改名后再跑，避免把 CSV 写进占位目录。
  endlocal & exit /b 1
)
echo(%NORM%| findstr /I /C:"/2026-10-26-day04-local" /C:"/2026-10-26-day05-loop-order-local" /C:"/2026-10-27-day07-blocking-local" /C:"/2026-11-01-compiler-blocking-windows" >nul
if not errorlevel 1 (
  echo REFUSE: output root is inside an archived results directory. Do not overwrite old CSV files.
  echo 拒绝：输出根落在已有归档结果目录里。不要覆盖旧 CSV。
  endlocal & exit /b 1
)
endlocal & exit /b 0

:checkmask
if "%P_CORE_MASK%"=="" (
  echo REFUSE: P_CORE_MASK is empty. Fill it from the core scan. Do not guess.
  echo 拒绝：P_CORE_MASK 是空的。请按核扫描填写，不要猜。
  exit /b 1
)
if /I "%P_CORE_MASK%"=="TODO" (
  echo REFUSE: P_CORE_MASK is still TODO. Read the core scan, then edit the top of this script.
  echo 拒绝：P_CORE_MASK 还是 TODO。先看核扫描，再改脚本顶部，不要猜一个 P 核。
  exit /b 1
)
if "%P_CORE_MASK%"=="0" (
  echo REFUSE: affinity mask 0 matches no logical processor.
  echo 拒绝：掩码 0 绑不到任何逻辑核。
  exit /b 1
)
if not "%P_CORE_MASK:~8%"=="" (
  echo REFUSE: P_CORE_MASK is longer than 8 hex digits.
  echo 拒绝：P_CORE_MASK 超过 8 位十六进制。这种 start /affinity 写法先不接受更长的掩码。
  exit /b 1
)
set "REST=%P_CORE_MASK%"
:hexloop
if "%REST%"=="" exit /b 0
set "CH=%REST:~0,1%"
set "REST=%REST:~1%"
echo(0123456789abcdefABCDEF| findstr /L /C:"%CH%" >nul
if errorlevel 1 (
  echo 拒绝：P_CORE_MASK 必须是十六进制，不带 0x。当前值不能用。
  exit /b 1
)
goto :hexloop

:print_plan
for /L %%R in (1,1,10) do (
  call :echo_one c0 %%R nobind
  call :echo_one c1 %%R bind
)
exit /b 0

:echo_one
setlocal
call :names %~1 %~2
if "%~3"=="bind" goto :echo_bind
echo mkdir "%DIR%"
echo cd /d "%DIR%"
echo start "" /wait "%EXE%"
goto :echo_done
:echo_bind
echo mkdir "%DIR%"
echo cd /d "%DIR%"
echo start "" /wait /high /affinity %P_CORE_MASK% "%EXE%"
:echo_done
endlocal & exit /b 0

:run_one
setlocal
call :names %~1 %~2
set "HOW=%~3"
if not exist "%DIR%" mkdir "%DIR%"
if not exist "%DIR%" (
  echo 无法创建 "%DIR%"
  endlocal & exit /b 1
)
set "HERE=%CD%"
cd /d "%DIR%"
if errorlevel 1 (
  echo 无法进入 "%DIR%"
  endlocal & exit /b 1
)
echo 当前目录 %CD%
if "%HOW%"=="bind" (
  echo C1 绑核掩码 %P_CORE_MASK%
  start "" /wait /high /affinity %P_CORE_MASK% "%EXE%"
) else (
  echo C0 不绑核
  start "" /wait "%EXE%"
)
set "RC=%ERRORLEVEL%"
cd /d "%HERE%"
if not "%RC%"=="0" (
  echo day07.exe 退出码 %RC%，停在 %DIR%。已经写出的目录留着。
  endlocal & exit /b %RC%
)
if not exist "%DIR%\matmul_benchmark_day7_blocking.csv" (
  echo 跑完后 "%DIR%" 里没有 matmul_benchmark_day7_blocking.csv。
  endlocal & exit /b 1
)
if exist "%HERE%\matmul_benchmark_day7_blocking.csv" (
  echo 拒绝继续：启动目录里出现了 matmul_benchmark_day7_blocking.csv。
  endlocal & exit /b 1
)
echo 完成 "%DIR%"
endlocal & exit /b 0

:names
set "WHICH=%~1"
set "RUN=%~2"
if %RUN% LSS 10 (
  set "NN=0%RUN%"
) else (
  set "NN=%RUN%"
)
set "DIR=%OUTROOT%\%WHICH%\run%NN%"
exit /b 0
