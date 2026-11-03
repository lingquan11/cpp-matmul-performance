@echo off
rem 这是什么：对 src\day07-matrix-blocking.cpp 做一次 /Qvec-report:2 + /FAs 检查。不计时。
rem 谁写的：脚手架由 AI 维护。day07 源码一行都不改。
rem 怎么运行：打开 x64 Native Tools Command Prompt，在仓库根目录：
rem   scripts\vec-report.bat
rem   scripts\vec-report.bat D:\tmp\simd-check
rem 不给目录时，输出到 experiments\results\<今天日期>-simd-factor-windows\。
rem 限制：
rem   提示词里的命令是 cl /O2 /Qvec-report:2 /FAs /c。
rem   这里只额外加了 /nologo、/Fo、/Fa，用来放好 .obj 和 .asm。
rem   V1 不加 /arch。V2 只多一个 /arch:AVX2。不加 /std，也不加别的优化开关。
rem   /FAs 是 /FA 这一组里「汇编里带源码」的写法。闸门只此一份，LDA 实验复用它，不另写脚本。
rem   云端没有 cl，不能确认 MSVC 19.51 是否打印 info C5001 / info C5002。
rem   Microsoft Learn 的 Vectorizer and parallelizer messages 记载 5001 是 Loop vectorized，
rem   5002 是 Loop not vectorized because of reason。命令以本机 cl 的实际输出为准。
rem   若本机没有这两条，保留 txt，不要改 day07 去凑消息。
rem   生成的 .asm、.obj、txt 先留在本机。要提交的话放进结果目录，并在 metadata 写编译器版本。
rem   占位目录 YYYY-MM-DD-simd-factor-windows 拒绝写入。

setlocal EnableExtensions
chcp 65001 >nul 2>&1

set "ROOT=%~dp0.."
pushd "%ROOT%"
set "ROOT=%CD%"
popd

where cl >nul 2>&1
if errorlevel 1 (
  echo 找不到 cl。请在 x64 Native Tools Command Prompt 里运行。
  echo 本脚本不在没有 MSVC 的环境里假装已经看过向量化报告。
  exit /b 1
)

if "%~1"=="" goto :default_out
set "OUT=%~f1"
goto :have_out

:default_out
set "TODAY="
for /f %%D in ('powershell -NoProfile -Command "Get-Date -Format yyyy-MM-dd"') do set "TODAY=%%D"
if "%TODAY%"=="" (
  echo 没有取到日期。请显式传入输出目录，例如 scripts\vec-report.bat D:\tmp\simd-check
  exit /b 2
)
set "OUT=%ROOT%\experiments\results\%TODAY%-simd-factor-windows"

:have_out
if "%OUT:~-1%"=="\" set "OUT=%OUT:~0,-1%"
for %%I in ("%OUT%") do set "LEAF=%%~nxI"
if /I "%LEAF%"=="YYYY-MM-DD-simd-factor-windows" (
  echo 拒绝：这是占位目录名。换成真实日期或别的目录，避免把汇编写进模板。
  exit /b 2
)
if exist "%OUT%\src\day07-matrix-blocking.cpp" (
  echo 拒绝：输出目录看起来是仓库根。
  exit /b 2
)
echo(%OUT%| findstr /I /C:"\2026-10-26-day04-local" /C:"\2026-10-26-day05-loop-order-local" /C:"\2026-10-27-day07-blocking-local" /C:"\2026-11-01-compiler-blocking-windows" >nul
if not errorlevel 1 (
  echo 拒绝：输出目录落在已有归档结果里。
  exit /b 2
)
if not exist "%ROOT%\src\day07-matrix-blocking.cpp" (
  echo 找不到 src\day07-matrix-blocking.cpp。请在仓库里运行。
  exit /b 2
)

if not exist "%OUT%\asm" mkdir "%OUT%\asm"
if not exist "%OUT%\asm" (
  echo 无法创建 "%OUT%\asm"
  exit /b 1
)

echo V1：cl /O2 /Qvec-report:2 /FAs /c  不加 /arch
cl /nologo /O2 /Qvec-report:2 /FAs /c /Fo"%OUT%\asm\day07-v1.obj" /Fa"%OUT%\asm\day07-v1.asm" "%ROOT%\src\day07-matrix-blocking.cpp" > "%OUT%\vec-report-v1.txt" 2>&1
set "RC=%ERRORLEVEL%"
if not "%RC%"=="0" (
  echo V1 编译失败，退出码 %RC%。原文在 "%OUT%\vec-report-v1.txt"。不要为了编过改 day07。
  exit /b %RC%
)

echo V2：同一条命令加上 /arch:AVX2
cl /nologo /O2 /arch:AVX2 /Qvec-report:2 /FAs /c /Fo"%OUT%\asm\day07-v2.obj" /Fa"%OUT%\asm\day07-v2.asm" "%ROOT%\src\day07-matrix-blocking.cpp" > "%OUT%\vec-report-v2.txt" 2>&1
set "RC=%ERRORLEVEL%"
if not "%RC%"=="0" (
  echo V2 编译失败，退出码 %RC%。原文在 "%OUT%\vec-report-v2.txt"。不要为了编过改 day07。
  exit /b %RC%
)

echo 编译检查写完了。这不是计时。
echo vec-report: "%OUT%\vec-report-v1.txt"
echo vec-report: "%OUT%\vec-report-v2.txt"
echo asm: "%OUT%\asm\day07-v1.asm"
echo asm: "%OUT%\asm\day07-v2.asm"
echo 接着可以跑：python scripts\scan-asm-keywords.py --asm "%OUT%\asm\day07-v1.asm" --vec-report "%OUT%\vec-report-v1.txt"
echo C5001/C5002 以这两个 txt 的原文为准。关键字计数不是向量化结论。
exit /b 0
