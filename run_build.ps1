# run_build.ps1
# ollie :: built by olivia :: https://discord.gg/ojailbreaks
# Idempotent rebuild helper. No admin needed once bootstrap.ps1 finished once.

$ErrorActionPreference = 'Stop'
$Root = Split-Path -Parent $MyInvocation.MyCommand.Path
Set-Location $Root

function Say($m) { Write-Host "[ollie] $m" -ForegroundColor Magenta }
function Die($m) { Write-Host "[ollie] $m" -ForegroundColor Red; exit 1 }
function Have($c) { try { $null = Get-Command $c -ErrorAction Stop; $true } catch { $false } }

if (-not (Have 'cmake')) { Die "cmake not on PATH -- run bootstrap.ps1 once (as admin)" }

$vswhere = "$env:ProgramFiles (x86)\Microsoft Visual Studio\Installer\vswhere.exe"
if (-not (Test-Path $vswhere)) { Die "vswhere missing -- run bootstrap.ps1 once (as admin)" }
$vs = & $vswhere -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath | Select-Object -First 1
if (-not $vs) { Die "no VS C++ toolchain -- run bootstrap.ps1 once (as admin)" }
$vcvars = Join-Path $vs 'VC\Auxiliary\Build\vcvars64.bat'
foreach ($line in (cmd /c "`"$vcvars`" >NUL && set")) {
    if ($line -match '^(?<k>[^=]+)=(?<v>.*)$') {
        [System.Environment]::SetEnvironmentVariable($matches['k'], $matches['v'], 'Process')
    }
}

$imgui   = Join-Path $Root 'payload\third_party\imgui\imgui.cpp'
$minhook = Join-Path $Root 'payload\third_party\minhook\include\MinHook.h'
if (-not (Test-Path $imgui))   { Die "imgui missing -- run bootstrap.ps1 once" }
if (-not (Test-Path $minhook)) { Die "minhook missing -- run bootstrap.ps1 once" }

$build = Join-Path $Root 'build'
Say "configure ..."
cmake -S $Root -B $build -A x64
if ($LASTEXITCODE -ne 0) { Die "configure failed" }
Say "build ..."
cmake --build $build --config Release -- /m
if ($LASTEXITCODE -ne 0) { Die "build failed" }

$exe = Join-Path $build 'Release\ollie.exe'
Say "ok -> $exe"
