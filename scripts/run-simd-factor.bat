@echo off
rem 由零醛按核扫描填写。十六进制，不带 0x。不要把耗时填在这里。
set "P_CORE_MASK=TODO"
rem 四个编译轮之间停多少秒。填 0 表示不停。
set "COOLDOWN_SEC=TODO"
set "RUNS=5"

setlocal EnableExtensions
rem 这是什么：按 V1、V2、V2、V1 的顺序交替编译和运行。
rem   V1 = x64 默认，不加 /arch（MSVC x64 文档里默认带 SSE2）。
rem   V2 = 同一条命令加上 /arch:AVX2。
rem   算法是 ikj、blocked_16、blocked_64，n 是 256 和 1024。
rem 谁写的：脚手架由 AI 维护。kernel 仍是 TODO(零醛)。
rem 怎么运行：在 x64 Native Tools 里
rem   scripts\run-simd-factor.bat ^<输出根目录^> [dry-run]
rem 限制：
rem   每一格目前是 1 个进程。规则 R 要 10 个进程才下快慢结论。
rem   规则全文在尚未合并的 ideas/matmul-1002-i1-measurement-baseline
rem   的 experiments\reports\measurement-rules.md。本脚本不把 24 次偷偷加成 240 次，
rem   也不用 5%% 的门槛写结论。要不要扩成 10 进程，由零醛定。
rem   V0（/DNO_VEC）是扩展，不在这四周里。手写 AVX2 intrinsic 也不是本脚本的事。
rem   绑核是 C1：start "" /wait /high /affinity。掩码不猜。
rem   不要预设 AVX2 一定快 8 倍。V1 接近 V2 本身也是发现。
rem   若 vec-report 说未向量化，只记录，不改源码。编译检查用 scripts\vec-report.bat，只做一次。
rem   占位目录 YYYY-MM-DD-simd-factor-windows 拒绝写入。
rem   云端、Linux 或虚拟机里的毫秒不能写进正式 CSV 或报告。
rem   n=1024 在 kernel 实现前，以及初始化公式定下来之前，exe 都会非零退出。

chcp 65001 >nul 2>&1

if "%~1"=="" goto :usage
if "%~2"=="" goto :args_ok
if /I "%~2"=="dry-run" goto :args_ok
echo 第二个参数只能是 dry-run，或者不写。
exit /b 2

:args_ok
set "ROOT=%~dp0.."
pushd "%ROOT%"
set "ROOT=%CD%"
popd
set "OUTROOT=%~f1"
set "MODE=%~2"
if "%OUTROOT:~-1%"=="\" set "OUTROOT=%OUTROOT:~0,-1%"

call :guard "%OUTROOT%"
if errorlevel 1 exit /b 2
call :checkmask
set "MASK_RC=%ERRORLEVEL%"
call :checkcool
set "COOL_RC=%ERRORLEVEL%"

if /I not "%MODE%"=="dry-run" (
  where cl >nul 2>&1
  if errorlevel 1 (
    echo 找不到 cl。请在 x64 Native Tools Command Prompt 里运行。
    exit /b 1
  )
  if not exist "%ROOT%\templates\gpt\simd-factor.cpp" (
    echo 找不到 templates\gpt\simd-factor.cpp
    exit /b 2
  )
  if not exist "%ROOT%\build" mkdir "%ROOT%\build"
)

if /I "%MODE%"=="dry-run" (
  echo dry-run：不编译，不创建目录，不启动 exe。
  call :passes
  if not "%MASK_RC%"=="0" (
    echo 真实运行会拒绝：P_CORE_MASK 还不能用。上面只是计划。
    exit /b 2
  )
  if not "%COOL_RC%"=="0" (
    echo 真实运行会拒绝：COOLDOWN_SEC 还是 TODO。上面只是计划。
    exit /b 2
  )
  echo dry-run 结束。cl 和 start 都没有执行。
  exit /b 0
)

if not "%MASK_RC%"=="0" exit /b 2
if not "%COOL_RC%"=="0" exit /b 2
call :passes
if errorlevel 1 exit /b 1
echo 四轮都跑完了。数字只以各目录里的 simd-factor.csv 为准。不要把 V1 接近 V2 先写成失败。
exit /b 0

:usage
echo 用法: %~nx0 ^<输出根目录^> [dry-run]
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
if /I "%LEAF%"=="YYYY-MM-DD-simd-factor-windows" (
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
if /I "%P_CORE_MASK%"=="TODO" (
  echo 拒绝：P_CORE_MASK 还是 TODO。先看核扫描，再改脚本顶部，不要猜一个 P 核。
  exit /b 1
)
if "%P_CORE_MASK%"=="" (
  echo 拒绝：P_CORE_MASK 是空的。
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

:checkcool
if /I "%COOLDOWN_SEC%"=="TODO" (
  echo 拒绝：COOLDOWN_SEC 还是 TODO。填 0 或正整数秒，不要猜。
  exit /b 1
)
if "%COOLDOWN_SEC%"=="" (
  echo 拒绝：COOLDOWN_SEC 是空的。
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

:passes
call :round 01 V1
if errorlevel 1 exit /b 1
call :cooldown
call :round 02 V2
if errorlevel 1 exit /b 1
call :cooldown
call :round 03 V2
if errorlevel 1 exit /b 1
call :cooldown
call :round 04 V1
if errorlevel 1 exit /b 1
exit /b 0

:round
set "PASS=%~1"
set "WHICH=%~2"
set "EXE=%ROOT%\build\simd-factor-%WHICH%.exe"
if /I "%MODE%"=="dry-run" goto :round_dry
if /I "%WHICH%"=="V2" (
  echo 编译 V2 /arch:AVX2
  cl /nologo /std:c++17 /EHsc /O2 /utf-8 /arch:AVX2 "%ROOT%\templates\gpt\simd-factor.cpp" /Fe:"%EXE%" /Fo:"%ROOT%\build\simd-factor-V2.obj"
) else (
  echo 编译 V1，不加 /arch
  cl /nologo /std:c++17 /EHsc /O2 /utf-8 "%ROOT%\templates\gpt\simd-factor.cpp" /Fe:"%EXE%" /Fo:"%ROOT%\build\simd-factor-V1.obj"
)
if errorlevel 1 (
  echo 编译失败，停在 %PASS% %WHICH%。不要为了编过改 kernel 以外的 day07。
  exit /b 1
)
call :grid
exit /b %ERRORLEVEL%
:round_dry
if /I "%WHICH%"=="V2" (
  echo cl /nologo /std:c++17 /EHsc /O2 /utf-8 /arch:AVX2 "%ROOT%\templates\gpt\simd-factor.cpp" /Fe:"%EXE%"
) else (
  echo cl /nologo /std:c++17 /EHsc /O2 /utf-8 "%ROOT%\templates\gpt\simd-factor.cpp" /Fe:"%EXE%"
)
call :grid
exit /b 0

:grid
if "%PASS%"=="01" goto :grid_a
if "%PASS%"=="03" goto :grid_a
call :cell blocked_64 1024
if errorlevel 1 exit /b 1
call :cell blocked_64 256
if errorlevel 1 exit /b 1
call :cell blocked_16 1024
if errorlevel 1 exit /b 1
call :cell blocked_16 256
if errorlevel 1 exit /b 1
call :cell ikj 1024
if errorlevel 1 exit /b 1
call :cell ikj 256
if errorlevel 1 exit /b 1
exit /b 0
:grid_a
call :cell ikj 256
if errorlevel 1 exit /b 1
call :cell ikj 1024
if errorlevel 1 exit /b 1
call :cell blocked_16 256
if errorlevel 1 exit /b 1
call :cell blocked_16 1024
if errorlevel 1 exit /b 1
call :cell blocked_64 256
if errorlevel 1 exit /b 1
call :cell blocked_64 1024
if errorlevel 1 exit /b 1
exit /b 0

:cell
setlocal
set "ALGO=%~1"
set "N=%~2"
set "DIR=%OUTROOT%\%PASS%-%WHICH%\%ALGO%\n%N%"
set "CSV=%DIR%\simd-factor.csv"
if /I "%MODE%"=="dry-run" goto :cell_dry
if not exist "%DIR%" mkdir "%DIR%"
if not exist "%DIR%" (
  echo 无法创建 "%DIR%"
  endlocal & exit /b 1
)
echo C1 绑核掩码 %P_CORE_MASK%  %PASS% %WHICH% %ALGO% n=%N%
start "" /wait /high /affinity %P_CORE_MASK% "%EXE%" --algo %ALGO% --n %N% --runs %RUNS% --out "%CSV%"
if errorlevel 1 (
  echo exe 退出码非零，停在 %DIR%。已经写出的目录留着。
  endlocal & exit /b 1
)
if not exist "%CSV%" (
  echo 跑完后没有 "%CSV%"。
  endlocal & exit /b 1
)
endlocal & exit /b 0
:cell_dry
echo mkdir "%DIR%"
echo start "" /wait /high /affinity %P_CORE_MASK% "%EXE%" --algo %ALGO% --n %N% --runs %RUNS% --out "%CSV%"
endlocal & exit /b 0

:cooldown
if /I "%MODE%"=="dry-run" exit /b 0
if "%COOLDOWN_SEC%"=="0" exit /b 0
echo 轮间冷却 %COOLDOWN_SEC% 秒
timeout /t %COOLDOWN_SEC% /nobreak >nul
exit /b 0
