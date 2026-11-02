@echo off
setlocal EnableExtensions
rem 这是什么：对逻辑核 0 到 19 各跑一次未改动的 day07.exe。
rem 谁写的：脚手架由 AI 维护。零醛在自己的 Windows 电脑上执行。
rem 怎么运行：
rem   scripts\run-core-scan.bat ^<day07.exe路径^> ^<输出根目录^> [dry-run]
rem   dry-run 只打印 mkdir、cd 和 start，不建目录，不启动 exe。
rem 限制：
rem   已核对 src\day07-matrix-blocking.cpp：main 没有参数。
rem   CSV 固定写到当前目录，文件名是 matmul_benchmark_day7_blocking.csv。
rem   所以每次都要先 mkdir，再 cd 进 core-scan\coreNN\，然后才 start exe。
rem   表头 12 列：n,algorithm,pilot_ms,sample,repeats,total_ms,ms_per_multiply,C00,expected_C00,checksum,elementwise_match,status。
rem   start /affinity 的掩码是十六进制，不带 0x、不带 h。核 k 的掩码是 1 左移 k 位。
rem   第一个引号是窗口标题，必须写成：
rem   start "" /wait /high /affinity ^<掩码^> "^<exe^>"
rem   dry-run 曾在 Wine 的 cmd 里核对过目录计划。没有执行 start，也没有在本机跑过 start /?。
rem   若本机不认 /affinity 的写法，走熔断。
rem   1.5 小时熔断：start /affinity 报错、exe 起不来，或 0 到 19 这一轮超过约 1.5 小时，
rem   立刻停。改用任务管理器手动设置相关性，只测 3 个核。
rem   在 metadata.md 写明是哪 3 个核、为什么没跑满。已经写出的 coreNN 留着，不要删掉重跑凑数。
rem   云端、Linux 或虚拟机里的毫秒不能写进正式 CSV 或报告。
rem   编译示例（不是这次的执行记录；metadata 的编译命令仍写实际那一次，没跑就写待测）：
rem   cl /nologo /std:c++17 /EHsc /O2 /utf-8 src\day07-matrix-blocking.cpp /Fe:build\day07.exe
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

if /I not "%MODE%"=="dry-run" (
  if not exist "%EXE%" (
    echo 找不到 exe："%EXE%"
    exit /b 2
  )
  if exist "%CD%\matmul_benchmark_day7_blocking.csv" (
    echo 拒绝：启动目录里已经有 matmul_benchmark_day7_blocking.csv。
    echo 请换到没有这份 CSV 的目录再运行，避免和旧文件混在一起。
    exit /b 2
  )
)

if /I "%MODE%"=="dry-run" (
  echo DRY-RUN: do not create directories and do not start the exe.
  echo dry-run：不创建目录，不启动 exe。
)

for /L %%C in (0,1,19) do (
  call :one %%C
  if errorlevel 1 exit /b 1
)

if /I "%MODE%"=="dry-run" (
  echo dry-run 结束。上面的 start 没有执行。
) else (
  echo 核 0 到 19 都跑完了。数字只以各 coreNN 目录里的 CSV 为准。
)
exit /b 0

:usage
echo 用法: %~nx0 ^<day07.exe路径^> ^<输出根目录^> [dry-run]
echo 每个进程一个目录：^<输出根^>\core-scan\core00 到 core19。
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

:one
setlocal
set /a CORE=%~1
if %CORE% LSS 10 (
  set "NN=0%CORE%"
) else (
  set "NN=%CORE%"
)
call :mask %CORE%
if errorlevel 1 (
  echo 无法为核 %CORE% 找到掩码。
  endlocal & exit /b 1
)
set "DIR=%OUTROOT%\core-scan\core%NN%"
if /I "%MODE%"=="dry-run" (
  echo mkdir "%DIR%"
  echo cd /d "%DIR%"
  echo start "" /wait /high /affinity %MASK% "%EXE%"
  endlocal & exit /b 0
)
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
echo 核 %CORE% 掩码 %MASK%
echo 当前目录 %CD%
start "" /wait /high /affinity %MASK% "%EXE%"
set "RC=%ERRORLEVEL%"
cd /d "%HERE%"
if not "%RC%"=="0" (
  echo day07.exe 退出码 %RC%，停在核 %CORE%。已经写出的目录留着。
  endlocal & exit /b %RC%
)
if not exist "%DIR%\matmul_benchmark_day7_blocking.csv" (
  echo 跑完后 "%DIR%" 里没有 matmul_benchmark_day7_blocking.csv。
  echo 请确认 exe 是 day07，并且 CSV 写在这个子目录，而不是仓库根。
  endlocal & exit /b 1
)
if exist "%HERE%\matmul_benchmark_day7_blocking.csv" (
  echo 拒绝继续：启动目录里出现了 matmul_benchmark_day7_blocking.csv。
  endlocal & exit /b 1
)
echo 完成 "%DIR%"
endlocal & exit /b 0

:mask
if "%~1"=="0" set "MASK=1" & exit /b 0
if "%~1"=="1" set "MASK=2" & exit /b 0
if "%~1"=="2" set "MASK=4" & exit /b 0
if "%~1"=="3" set "MASK=8" & exit /b 0
if "%~1"=="4" set "MASK=10" & exit /b 0
if "%~1"=="5" set "MASK=20" & exit /b 0
if "%~1"=="6" set "MASK=40" & exit /b 0
if "%~1"=="7" set "MASK=80" & exit /b 0
if "%~1"=="8" set "MASK=100" & exit /b 0
if "%~1"=="9" set "MASK=200" & exit /b 0
if "%~1"=="10" set "MASK=400" & exit /b 0
if "%~1"=="11" set "MASK=800" & exit /b 0
if "%~1"=="12" set "MASK=1000" & exit /b 0
if "%~1"=="13" set "MASK=2000" & exit /b 0
if "%~1"=="14" set "MASK=4000" & exit /b 0
if "%~1"=="15" set "MASK=8000" & exit /b 0
if "%~1"=="16" set "MASK=10000" & exit /b 0
if "%~1"=="17" set "MASK=20000" & exit /b 0
if "%~1"=="18" set "MASK=40000" & exit /b 0
if "%~1"=="19" set "MASK=80000" & exit /b 0
exit /b 1
