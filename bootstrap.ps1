# bootstrap.ps1
# ollie :: built by olivia :: https://discord.gg/ojailbreaks
#
# One-shot: install CMake + VS 2022 Build Tools (C++) if missing,
# fetch ImGui 1.90.4 + MinHook 1.3.3 into payload\third_party\,
# configure and build ollie.exe.
#
# First run: elevated PowerShell (Right-click PowerShell -> Run as administrator).
# Later rebuilds: use run_build.ps1 (no admin needed).

$ErrorActionPreference = 'Stop'
$Root = Split-Path -Parent $MyInvocation.MyCommand.Path
Set-Location $Root

function Say($m) { Write-Host "[ollie] $m" -ForegroundColor Magenta }
function Die($m) { Write-Host "[ollie] $m" -ForegroundColor Red; exit 1 }

function Test-Admin {
    $id = [System.Security.Principal.WindowsIdentity]::GetCurrent()
    $p  = New-Object System.Security.Principal.WindowsPrincipal($id)
    return $p.IsInRole([System.Security.Principal.WindowsBuiltInRole]::Administrator)
}

function Elevate-Self {
    Write-Host "[ollie] not elevated. relaunching as admin -- accept the UAC prompt." -ForegroundColor Yellow
    $script = $MyInvocation.PSCommandPath
    if (-not $script) { $script = $PSCommandPath }
    if (-not $script) { $script = (Join-Path (Get-Location) 'bootstrap.ps1') }
    $args = @(
        '-NoProfile',
        '-ExecutionPolicy', 'Bypass',
        '-NoExit',
        '-File', "`"$script`""
    )
    Start-Process powershell.exe -Verb RunAs -ArgumentList $args
    exit 0
}

if (-not (Test-Admin)) { Elevate-Self }

function Refresh-Path {
    $m = [System.Environment]::GetEnvironmentVariable('Path','Machine')
    $u = [System.Environment]::GetEnvironmentVariable('Path','User')
    $env:Path = "$m;$u"
}

function Have($cmd) {
    try { $null = Get-Command $cmd -ErrorAction Stop; return $true } catch { return $false }
}

function Winget-Install($id, $label) {
    Say "installing $label ($id) ..."
    winget install --id $id --silent --accept-package-agreements --accept-source-agreements --disable-interactivity | Out-Host
    if ($LASTEXITCODE -ne 0 -and $LASTEXITCODE -ne -1978335189) {
        Die "winget failed for $id (exit $LASTEXITCODE)"
    }
    Refresh-Path
}

# ---------- toolchain ----------

Say "fridge hum. bench lit."
Say "built by olivia -- https://discord.gg/ojailbreaks"

if (-not (Have 'winget')) {
    Die "winget not present. install 'App Installer' from the Microsoft Store, then rerun."
}

# CMake
if (-not (Have 'cmake')) {
    Winget-Install 'Kitware.CMake' 'CMake'
}
if (-not (Have 'cmake')) {
    $cmakeGuess = "$env:ProgramFiles\CMake\bin"
    if (Test-Path $cmakeGuess) { $env:Path = "$cmakeGuess;$env:Path" }
}
if (-not (Have 'cmake')) { Die "cmake still not on PATH after install. open a new shell and try again." }
Say ("cmake: " + (cmake --version | Select-Object -First 1))

# VS Build Tools + MSVC C++
$vswhere = "$env:ProgramFiles (x86)\Microsoft Visual Studio\Installer\vswhere.exe"
function Find-VsInstall {
    if (-not (Test-Path $vswhere)) { return $null }
    & $vswhere -latest -products * `
        -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 `
        -property installationPath 2>$null | Select-Object -First 1
}

$vs = Find-VsInstall
if (-not $vs) {
    Say "installing Visual Studio 2022 Build Tools (C++)..."
    $args = '--passive --wait --norestart ' +
            '--add Microsoft.VisualStudio.Workload.VCTools ' +
            '--add Microsoft.VisualStudio.Component.VC.Tools.x86.x64 ' +
            '--add Microsoft.VisualStudio.Component.Windows11SDK.22621 ' +
            '--includeRecommended'
    winget install --id Microsoft.VisualStudio.2022.BuildTools --silent `
        --accept-package-agreements --accept-source-agreements `
        --override $args | Out-Host
    if ($LASTEXITCODE -ne 0) { Die "VS Build Tools install returned $LASTEXITCODE" }
    Refresh-Path
    $vs = Find-VsInstall
    if (-not $vs) { Die "VS install finished but vswhere still can't see MSVC. reboot and rerun." }
}
Say "VS install: $vs"

# vcvars64 lets cmake find the x64 MSVC compiler
$vcvars = Join-Path $vs 'VC\Auxiliary\Build\vcvars64.bat'
if (-not (Test-Path $vcvars)) { Die "vcvars64.bat missing under $vs" }

# import vcvars env into this PS session
Say "importing vcvars64 env ..."
$envDump = cmd /c "`"$vcvars`" >NUL && set"
foreach ($line in $envDump) {
    if ($line -match '^(?<k>[^=]+)=(?<v>.*)$') {
        [System.Environment]::SetEnvironmentVariable($matches['k'], $matches['v'], 'Process')
    }
}

# ---------- dependencies ----------

$tp = Join-Path $Root 'payload\third_party'
New-Item -ItemType Directory -Force -Path $tp | Out-Null
$imguiDst   = Join-Path $tp 'imgui'
$minhookDst = Join-Path $tp 'minhook'

function Fetch-Zip($url, $dst) {
    $tmp = Join-Path $env:TEMP ([System.IO.Path]::GetRandomFileName() + '.zip')
    Say "GET $url"
    Invoke-WebRequest -Uri $url -OutFile $tmp -UseBasicParsing
    $xtract = Join-Path $env:TEMP ([System.IO.Path]::GetRandomFileName())
    New-Item -ItemType Directory -Force -Path $xtract | Out-Null
    Expand-Archive -Path $tmp -DestinationPath $xtract -Force
    Remove-Item $tmp -Force
    $inner = Get-ChildItem $xtract -Directory | Select-Object -First 1
    if (-not $inner) { Die "zip from $url had no top-level directory" }
    if (Test-Path $dst) { Remove-Item $dst -Recurse -Force }
    Move-Item $inner.FullName $dst -Force
    Remove-Item $xtract -Recurse -Force
}

if (-not (Test-Path (Join-Path $imguiDst 'imgui.cpp'))) {
    Fetch-Zip 'https://github.com/ocornut/imgui/archive/refs/tags/v1.90.4.zip' $imguiDst
} else { Say "imgui already present" }

if (-not (Test-Path (Join-Path $minhookDst 'src\hook.c'))) {
    Fetch-Zip 'https://github.com/TsudaKageyu/minhook/archive/refs/tags/v1.3.3.zip' $minhookDst
} else { Say "minhook already present" }

# sanity
$need = @(
    "$imguiDst\imgui.cpp",
    "$imguiDst\backends\imgui_impl_dx11.cpp",
    "$imguiDst\backends\imgui_impl_win32.cpp",
    "$minhookDst\include\MinHook.h",
    "$minhookDst\src\hook.c"
)
foreach ($p in $need) { if (-not (Test-Path $p)) { Die "missing after fetch: $p" } }
Say "deps staged."

# ---------- configure + build ----------

$build = Join-Path $Root 'build'
Say "cmake configure ..."
cmake -S $Root -B $build -A x64
if ($LASTEXITCODE -ne 0) { Die "cmake configure failed" }

Say "cmake build ..."
cmake --build $build --config Release -- /m
if ($LASTEXITCODE -ne 0) { Die "cmake build failed" }

$exe = Join-Path $build 'Release\ollie.exe'
if (-not (Test-Path $exe)) { Die "no ollie.exe at $exe" }

Say "cheeks in. output: $exe"
Say "ship this one file. run it while fivem is up."
