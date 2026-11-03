@echo off
rem 由零醛按核扫描填写。十六进制，不带 0x。不要把耗时填在这里。
set "P_CORE_MASK=TODO"
rem 轮与轮之间停多少秒。填 0 表示不停。不要猜一个「看起来合理」的秒数。
set "COOLDOWN_SEC=TODO"
rem 每个进程里的正式样本数，不是外层的 10 个进程。
set "RUNS=5"

setlocal EnableExtensions
rem 这是什么：lda x n x algo，各 10 个进程，C1（绑一个 P 核并加 /high），独立目录。
rem 谁写的：脚手架由 AI 维护。kernel 仍是 TODO(零醛)。P 核掩码由零醛填。
rem 怎么运行：
rem   scripts\run-layout-lda.bat ^<layout-lda.exe^> ^<输出根目录^> [dry-run]
rem 限制：
rem   C1 的写法与尚未合并的 ideas/matmul-1002-i1-measurement-baseline
rem   里 scripts\run-c0-c1.bat 相同：start "" /wait /high /affinity ^<掩码^>。
rem   规则 R 的全文在那一支的 experiments\reports\measurement-rules.md，本脚本不复制。
rem   那份草案里 C0/C1 合计 5 小时的熔断是那次实验自己的，不自动套到这里。
rem   本脚本不另写一个小时数。
rem   n=256 时 lda 记号 n 和 256 是同一个数，只跑 n，目录名 lda-n。n+16 是 272。
rem   奇数进程轮：先 ijk，lda 顺序 n、n+16、256。偶数轮对调算法，lda 顺序反过来。
rem   核心未实现时 exe 非零退出，脚本停下。这是预期，不要改脚本去补一个假 kernel。
rem   占位目录 YYYY-MM-DD-layout-lda-windows 拒绝写入。不覆盖已有归档 CSV。
rem   云端、Linux 或虚拟机里的毫秒不能写进正式 CSV 或报告。

chcp 65001 >nul 2>&1

if "%~1"=="" goto :usage
if "%~2"=="" goto :usage
if "%~3"=="" goto :args_ok
if /I "%~3"=="dry-run" goto :args_ok
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
call :checkcool
set "COOL_RC=%ERRORLEVEL%"

if /I "%MODE%"=="dry-run" (
  echo dry-run：不创建目录，不启动 exe。
  call :allruns
  if not "%MASK_RC%"=="0" (
    echo 真实运行会拒绝：P_CORE_MASK 还不能用。上面只是计划。
    exit /b 2
  )
  if not "%COOL_RC%"=="0" (
    echo 真实运行会拒绝：COOLDOWN_SEC 还是 TODO。上面只是计划。
    exit /b 2
  )
  echo dry-run 结束。start 没有执行。
  exit /b 0
)

if not "%MASK_RC%"=="0" exit /b 2
if not "%COOL_RC%"=="0" exit /b 2
if not exist "%EXE%" (
  echo 找不到 exe："%EXE%"
  exit /b 2
)
call :allruns
if errorlevel 1 exit /b 1
echo 10 个进程都跑完了。数字只以各自 runNN 目录里的 layout-lda.csv 为准。
exit /b 0

:usage
echo 用法: %~nx0 ^<layout-lda.exe^> ^<输出根目录^> [dry-run]
echo 先把脚本顶部的 P_CORE_MASK 和 COOLDOWN_SEC 改成你的值。
exit /b 2

:guard
setlocal
set "ROOT=%~1"
for %%I in ("%ROOT%") do set "LEAF=%%~nxI"
if exist "%ROOT%\src\day07-matrix-blocking.cpp" (
  echo 拒绝：输出根是仓库根。
  endlocal & exit /b 1
)
if /I "%LEAF%"=="results" (
  echo 拒绝：不要把 experiments\results 本身当输出根。
  endlocal & exit /b 1
)
if /I "%LEAF%"=="YYYY-MM-DD-layout-lda-windows" (
  echo 拒绝：模板目录还没改成真实日期。
  endlocal & exit /b 1
)
echo(%ROOT%| findstr /I /C:"\2026-10-26-day04-local" /C:"\2026-10-26-day05-loop-order-local" /C:"\2026-10-27-day07-blocking-local" /C:"\2026-11-01-compiler-blocking-windows" >nul
if not errorlevel 1 (
  echo 拒绝：输出根落在已有归档结果目录里。
  endlocal & exit /b 1
)
endlocal & exit /b 0

:checkmask
if "%P_CORE_MASK%"=="" goto :mask_bad
if /I "%P_CORE_MASK%"=="TODO" (
  echo 拒绝：P_CORE_MASK 还是 TODO。先看核扫描，再改脚本顶部，不要猜一个 P 核。
  exit /b 1
)
if "%P_CORE_MASK%"=="0" (
  echo 拒绝：掩码 0 绑不到任何逻辑核。
  exit /b 1
)
if not "%P_CORE_MASK:~8%"=="" (
  echo 拒绝：P_CORE_MASK 超过 8 位十六进制。
  exit /b 1
)
set "REST=%P_CORE_MASK%"
:hexloop
if "%REST%"=="" exit /b 0
set "CH=%REST:~0,1%"
set "REST=%REST:~1%"
echo(0123456789abcdefABCDEF| findstr /L /C:"%CH%" >nul
if errorlevel 1 (
  echo 拒绝：P_CORE_MASK 必须是十六进制，不带 0x。
  exit /b 1
)
goto :hexloop
:mask_bad
echo 拒绝：P_CORE_MASK 是空的。
exit /b 1

:checkcool
if "%COOLDOWN_SEC%"=="" goto :cool_bad
if /I "%COOLDOWN_SEC%"=="TODO" (
  echo 拒绝：COOLDOWN_SEC 还是 TODO。填 0 或正整数秒，不要猜。
  exit /b 1
)
set "REST=%COOLDOWN_SEC%"
:coolloop
if "%REST%"=="" exit /b 0
set "CH=%REST:~0,1%"
set "REST=%REST:~1%"
echo(0123456789| findstr /L /C:"%CH%" >nul
if errorlevel 1 (
  echo 拒绝：COOLDOWN_SEC 必须是十进制秒。
  exit /b 1
)
goto :coolloop
:cool_bad
echo 拒绝：COOLDOWN_SEC 是空的。
exit /b 1

:allruns
for /L %%R in (1,1,10) do (
  call :dispatch %%R
  if errorlevel 1 exit /b 1
)
exit /b 0

:dispatch
set /a "ODD=%~1 %% 2"
if "%ODD%"=="0" goto :even
call :odd %~1
exit /b %ERRORLEVEL%

:odd
call :cell %1 64 n ijk
if errorlevel 1 exit /b 1
call :cell %1 64 n+16 ijk
if errorlevel 1 exit /b 1
call :cell %1 64 256 ijk
if errorlevel 1 exit /b 1
call :cell %1 64 n ikj
if errorlevel 1 exit /b 1
call :cell %1 64 n+16 ikj
if errorlevel 1 exit /b 1
call :cell %1 64 256 ikj
if errorlevel 1 exit /b 1
call :cell %1 128 n ijk
if errorlevel 1 exit /b 1
call :cell %1 128 n+16 ijk
if errorlevel 1 exit /b 1
call :cell %1 128 256 ijk
if errorlevel 1 exit /b 1
call :cell %1 128 n ikj
if errorlevel 1 exit /b 1
call :cell %1 128 n+16 ikj
if errorlevel 1 exit /b 1
call :cell %1 128 256 ikj
if errorlevel 1 exit /b 1
call :cell %1 256 n ijk
if errorlevel 1 exit /b 1
call :cell %1 256 n+16 ijk
if errorlevel 1 exit /b 1
call :cell %1 256 n ikj
if errorlevel 1 exit /b 1
call :cell %1 256 n+16 ikj
if errorlevel 1 exit /b 1
exit /b 0

:even
call :cell %1 64 256 ikj
if errorlevel 1 exit /b 1
call :cell %1 64 n+16 ikj
if errorlevel 1 exit /b 1
call :cell %1 64 n ikj
if errorlevel 1 exit /b 1
call :cell %1 64 256 ijk
if errorlevel 1 exit /b 1
call :cell %1 64 n+16 ijk
if errorlevel 1 exit /b 1
call :cell %1 64 n ijk
if errorlevel 1 exit /b 1
call :cell %1 128 256 ikj
if errorlevel 1 exit /b 1
call :cell %1 128 n+16 ikj
if errorlevel 1 exit /b 1
call :cell %1 128 n ikj
if errorlevel 1 exit /b 1
call :cell %1 128 256 ijk
if errorlevel 1 exit /b 1
call :cell %1 128 n+16 ijk
if errorlevel 1 exit /b 1
call :cell %1 128 n ijk
if errorlevel 1 exit /b 1
call :cell %1 256 n+16 ikj
if errorlevel 1 exit /b 1
call :cell %1 256 n ikj
if errorlevel 1 exit /b 1
call :cell %1 256 n+16 ijk
if errorlevel 1 exit /b 1
call :cell %1 256 n ijk
if errorlevel 1 exit /b 1
exit /b 0

:cell
setlocal
set "RUN=%~1"
set "N=%~2"
set "TOKEN=%~3"
set "ALGO=%~4"
set "NN=0%RUN%"
if %RUN% GEQ 10 set "NN=%RUN%"
set "LDALABEL=lda-n"
if "%TOKEN%"=="n+16" set "LDALABEL=lda-n16"
if "%TOKEN%"=="256" set "LDALABEL=lda-256"
set "DIR=%OUTROOT%\n%N%\%LDALABEL%\%ALGO%\run%NN%"
set "CSV=%DIR%\layout-lda.csv"
if /I "%MODE%"=="dry-run" goto :cell_dry
if not exist "%DIR%" mkdir "%DIR%"
if not exist "%DIR%" (
  echo 无法创建 "%DIR%"
  endlocal & exit /b 1
)
echo C1 绑核掩码 %P_CORE_MASK%  目录 %DIR%
start "" /wait /high /affinity %P_CORE_MASK% "%EXE%" --algo %ALGO% --n %N% --lda %TOKEN% --runs %RUNS% --out "%CSV%"
if errorlevel 1 (
  echo exe 退出码非零，停在 %DIR%。已经写出的目录留着。
  endlocal & exit /b 1
)
if not exist "%CSV%" (
  echo 跑完后没有 "%CSV%"。
  endlocal & exit /b 1
)
call :cooldown
endlocal & exit /b 0
:cell_dry
echo mkdir "%DIR%"
echo start "" /wait /high /affinity %P_CORE_MASK% "%EXE%" --algo %ALGO% --n %N% --lda %TOKEN% --runs %RUNS% --out "%CSV%"
endlocal & exit /b 0

:cooldown
if "%COOLDOWN_SEC%"=="0" exit /b 0
timeout /t %COOLDOWN_SEC% /nobreak >nul
exit /b 0
