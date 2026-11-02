# 六种循环顺序的 C1 运行脚手架。
#
# 它是什么：在 Windows 上按 n∈{128,256} × 六种顺序 × 10 个进程启动
# templates/gpt/exp-loop-orders.cpp 编出来的程序。
# 谁写的：脚手架由 AI 维护。四种新 kernel 仍是 TODO(零醛)。
# 怎么运行（仓库根目录，x64 Native Tools 或已能启动 exe 的 PowerShell）：
#   powershell -NoProfile -File scripts\run-loop-orders-c1.ps1 -ListOnly
# ListOnly 不带 -Runs 时，清单里的 runs 只显示成 1，方便看命令，不代表已经选定重复次数，也不启动进程。
#   powershell -NoProfile -File scripts\run-loop-orders-c1.ps1 -Exe build\exp-loop-orders.exe -Runs 1 -OutDir build\loop-orders-c1
# 限制：
# - 不把输出放进 experiments/results/。云端或 Linux 的毫秒不能写入正式 CSV 或报告。
# - 未实现的顺序由 exe 非零退出。本脚本只记录退出码，不填毫秒，不写成 PASS。
# - 不含 SCALE-Sim。
# - 默认只查询 powercfg，不改电源计划。-SwitchHighPerformance 会改本机方案，待零醛确认后再用。
#
# C1 在本脚本里指下面这些过程条件。要改哪里，待零醛确认：
# 1. n 只取 128 和 256。
# 2. 顺序为 ijk、ikj、jik、kij、jki、kji。
# 3. 每种组合 10 个独立进程，依次启动，不并行。一进程一个 --out 文件。
# 4. 进程亲和性绑到逻辑处理器 0（mask 1）。它是不是 P-core，待零醛在本机确认。
# 5. 打印 powercfg /getactivescheme。默认不切换方案。
# 6. 进程内重复次数由 -Runs 传入，脚本不把某个毫秒写成已测结果。

[CmdletBinding()]
param(
    [string]$Exe = "build\exp-loop-orders.exe",
    [int]$Runs = 0,
    [string]$OutDir = "build\loop-orders-c1",
    [switch]$ListOnly,
    [switch]$SwitchHighPerformance
)

$ErrorActionPreference = "Stop"

$sizes = @(128, 256)
$algos = @("ijk", "ikj", "jik", "kij", "jki", "kji")
$processCount = 10

function Quote-Arg([string]$Text) {
    if ($Text -match '[\s"]') {
        return '"' + ($Text -replace '"', '\"') + '"'
    }
    return $Text
}

function Test-ArchivedResults([string]$Path) {
    $full = [System.IO.Path]::GetFullPath($Path)
    $normalized = ($full -replace '\\', '/').TrimEnd('/') + '/'
    return $normalized.Contains("/experiments/results/")
}

if (-not $ListOnly -and ($Runs -lt 1 -or $Runs -gt 100000)) {
    Write-Host "-Runs 必须是 1..100000 的整数。这是守卫，不是已测的重复次数。"
    exit 2
}
if ($ListOnly -and $Runs -eq 0) {
    $Runs = 1
}

if (Test-ArchivedResults $OutDir) {
    Write-Host "拒绝把 OutDir 放进 experiments/results/。"
    exit 2
}

Write-Host "C1 计划：n=128,256 × algo=ijk,ikj,jik,kij,jki,kji × 10 个顺序进程。runs=$Runs。"
Write-Host "亲和性 mask=1（逻辑处理器 0）。是否为 P-core：待零醛确认。"
Write-Host "计数与排名都不在本脚本里产生。未实现的 kernel 保持非零退出。"

if ($ListOnly) {
    foreach ($n in $sizes) {
        foreach ($algo in $algos) {
            foreach ($index in 1..$processCount) {
                $name = "n{0}-{1}-p{2:d2}.csv" -f $n, $algo, $index
                Write-Host ("LIST {0} --algo {1} --n {2} --runs {3} --out {4}" -f $Exe, $algo, $n, $Runs, (Join-Path $OutDir $name))
            }
        }
    }
    Write-Host "ListOnly：120 条命令，没有启动进程。"
    exit 0
}

if (-not (Test-Path -LiteralPath $Exe)) {
    Write-Host "找不到 $Exe。先在本机编译 templates\gpt\exp-loop-orders.cpp。"
    exit 2
}

New-Item -ItemType Directory -Force -Path $OutDir | Out-Null
$schemePath = Join-Path $OutDir "power-scheme.txt"
Write-Host "C1 电源计划（只查询，除非另外打开了 -SwitchHighPerformance）："
$scheme = & powercfg /getactivescheme
$scheme | Tee-Object -FilePath $schemePath
if ($SwitchHighPerformance) {
    Write-Host "待零醛确认：现在把活动电源方案切到 SCHEME_MIN（高性能）。这会改本机设置。"
    & powercfg /setactive SCHEME_MIN
    & powercfg /getactivescheme | Tee-Object -FilePath $schemePath -Append
}

$exePath = (Resolve-Path -LiteralPath $Exe).Path
$failures = New-Object System.Collections.Generic.List[string]
foreach ($n in $sizes) {
    foreach ($algo in $algos) {
        foreach ($index in 1..$processCount) {
            $fileName = "n{0}-{1}-p{2:d2}.csv" -f $n, $algo, $index
            $outFile = Join-Path $OutDir $fileName
            if (Test-ArchivedResults $outFile) {
                Write-Host "拒绝写入 $outFile"
                exit 2
            }
            $argumentLine = @(
                (Quote-Arg "--algo"),
                (Quote-Arg $algo),
                (Quote-Arg "--n"),
                (Quote-Arg "$n"),
                (Quote-Arg "--runs"),
                (Quote-Arg "$Runs"),
                (Quote-Arg "--out"),
                (Quote-Arg $outFile)
            ) -join " "
            $process = New-Object System.Diagnostics.Process
            $process.StartInfo.FileName = $exePath
            $process.StartInfo.Arguments = $argumentLine
            $process.StartInfo.UseShellExecute = $false
            $process.StartInfo.WorkingDirectory = (Get-Location).Path
            [void]$process.Start()
            try {
                $process.ProcessorAffinity = [IntPtr]1
            } catch {
                Write-Host "设置亲和性失败。待零醛确认逻辑处理器 0 是否可用。$($_.Exception.Message)"
                exit 1
            }
            $process.WaitForExit()
            $code = $process.ExitCode
            if ($code -ne 0) {
                $line = "exit=$code algo=$algo n=$n process=$index"
                $failures.Add($line)
                Write-Host "未记为 PASS：$line"
            } else {
                Write-Host "进程退出 0：algo=$algo n=$n process=$index。毫秒留在该 csv，正式归档前不要收进报告。"
            }
        }
    }
}

$failurePath = Join-Path $OutDir "failures.txt"
if ($failures.Count -gt 0) {
    $failures | Set-Content -Encoding utf8 -Path $failurePath
    Write-Host "有 $($failures.Count) 个进程非零退出。详见 $failurePath。没有为这些进程填写毫秒。"
    exit 1
}

Write-Host "120 个进程退出码都是 0。正式样本仍要零醛决定是否另建 results 目录并写 metadata。"
exit 0
