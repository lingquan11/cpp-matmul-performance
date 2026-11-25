@echo off
rem 向量化报告：同一个源文件分别用 /O2 和 /O2 /arch:AVX2 编译，保存 /Qvec-report:2 原文和带源码的汇编。不计时。
rem 用法（x64 Native Tools Command Prompt，仓库根目录）：
rem   results\scripts\vec-report.bat [源文件，默认 results\src\large-matrix.cpp] [输出目录，默认 results\experiments\05-simd]
rem 本机 MSVC 必须加 /utf-8（源文件含中文注释）。C5001 = 已向量化，C5002 = 未向量化及原因代码，以 txt 原文为准。
setlocal
chcp 65001 >nul 2>&1
where cl >nul 2>&1 || (echo 找不到 cl：请在 x64 Native Tools Command Prompt 里运行 & exit /b 1)
set "SRC=%~1"
if "%SRC%"=="" set "SRC=results\src\large-matrix.cpp"
set "OUT=%~2"
if "%OUT%"=="" set "OUT=results\experiments\05-simd"
if not exist "%SRC%" (echo 找不到 %SRC% & exit /b 2)
if not exist "%OUT%\asm" mkdir "%OUT%\asm"
for %%F in ("%SRC%") do set "NAME=%%~nF"

cl /nologo /utf-8 /O2 /Qvec-report:2 /FAs /c /Fo"%OUT%\asm\%NAME%-o2.obj" /Fa"%OUT%\asm\%NAME%-o2.asm" "%SRC%" > "%OUT%\vec-report-o2.txt" 2>&1 || (echo /O2 编译失败，见 %OUT%\vec-report-o2.txt & exit /b 1)
cl /nologo /utf-8 /O2 /arch:AVX2 /Qvec-report:2 /FAs /c /Fo"%OUT%\asm\%NAME%-avx2.obj" /Fa"%OUT%\asm\%NAME%-avx2.asm" "%SRC%" > "%OUT%\vec-report-avx2.txt" 2>&1 || (echo /arch:AVX2 编译失败，见 %OUT%\vec-report-avx2.txt & exit /b 1)

echo 报告：%OUT%\vec-report-o2.txt、%OUT%\vec-report-avx2.txt
echo 汇编：%OUT%\asm\%NAME%-o2.asm、%OUT%\asm\%NAME%-avx2.asm（findstr /n "ymm" 查向量宽度）
exit /b 0
