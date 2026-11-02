# AI 维护：运行单个隔离进程并强制超时；不负责正式实验条件选择。
param([string]$Exe,[string]$Directory,[string]$Mask,[int]$N,[string]$Algorithm,[int]$TimeoutSec)
$ErrorActionPreference = 'Stop'
$exePath = (Resolve-Path -LiteralPath $Exe).Path
$dirPath = (Resolve-Path -LiteralPath $Directory).Path
$outPath = Join-Path $dirPath 'sample.csv'
# cmd start /wait 返回子进程退出码；通过进程树强制结束，避免遗留计算。
$arguments = '/d /s /c "start "" /wait /affinity ' + $Mask + ' /high "' + $exePath + '" --n ' + $N + ' --algo ' + $Algorithm + ' --runs 5 --out "' + $outPath + '" --timeout-sec ' + $TimeoutSec + '"'
$proc = Start-Process -FilePath $env:ComSpec -ArgumentList $arguments -WorkingDirectory $dirPath -PassThru -WindowStyle Hidden
if (-not $proc.WaitForExit($TimeoutSec * 1000)) {
    & taskkill /PID $proc.Id /T /F | Out-Null
    $header='n,algorithm,pilot_ms,sample,repeats,total_ms,ms_per_multiply,C00,expected_C00,checksum,elementwise_match,status,ld,block_size,gmac_per_s,skipped_timeout'
    $block=if ($Algorithm -eq 'blocked_64') {64} elseif ($Algorithm -eq 'blocked_128') {128} else {0}
    @($header,"$N,$Algorithm,,,,,,,,,,$N,$block,,1") | Set-Content -LiteralPath $outPath -Encoding ascii
    Write-Host "Timeout: $N $Algorithm; recorded skipped_timeout=1."
    exit 0
}
exit $proc.ExitCode
