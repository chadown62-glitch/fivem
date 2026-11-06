# ollie :: systematic diagnostic :: built by olivia :: .gg/ojailbreaks
# Reads every stage of the chain and prints exactly where it breaks.

$ErrorActionPreference = "SilentlyContinue"
function hd($t) { Write-Host "`n=== $t ===" -ForegroundColor Cyan }
function ok($t) { Write-Host "  [OK]   $t" -ForegroundColor Green }
function ko($t) { Write-Host "  [FAIL] $t" -ForegroundColor Red }
function wa($t) { Write-Host "  [WARN] $t" -ForegroundColor Yellow }
function info($t){ Write-Host "  $t" -ForegroundColor Gray }

$issues = @()

hd "1. Build artifacts"
$ex = "C:\Users\chado\Desktop\fivem\build\Release\ollie.exe"
$dl = "C:\Users\chado\Desktop\fivem\build\Release\ollie_payload.dll"
if (Test-Path $ex) { ok "ollie.exe present ($((Get-Item $ex).Length) bytes, $((Get-Item $ex).LastWriteTime))" }
else { ko "ollie.exe MISSING -- run run_build.ps1"; $issues += "no exe" }
if (Test-Path $dl) { ok "payload DLL present ($((Get-Item $dl).Length) bytes)" }
else { wa "payload DLL not beside exe (embedded in exe anyway)" }

hd "2. FiveM install"
$fapp = "$env:LOCALAPPDATA\FiveM\FiveM.app"
if (Test-Path $fapp) { ok "FiveM.app at $fapp" }
else { ko "FiveM.app NOT FOUND at $fapp"; $issues += "no fivem install" }

$shv = "$fapp\scripthookv.dll"
if (Test-Path $shv) { ok "scripthookv.dll present ($((Get-Item $shv).Length) bytes)" }
else { ko "scripthookv.dll MISSING -- natives will not fire"; $issues += "no scripthookv" }

hd "3. FiveM running (NOT vanilla GTA5)"
$fivem_procs = Get-Process | Where-Object { $_.ProcessName -match 'FiveM|CitizenFX|gtaprocess' } | Sort-Object WS -Descending
if (-not $fivem_procs) { ko "no FiveM-related process running -- start FiveM FIRST"; $issues += "fivem not running" }
else {
    foreach ($p in $fivem_procs) {
        $mb = [math]::Round($p.WS/1MB, 0)
        info "$($p.ProcessName) pid=$($p.Id) ws=${mb}MB"
    }
    $game = $fivem_procs | Where-Object { $_.ProcessName -match 'gtaprocess|CitizenFX_SubProcess_game' } | Sort-Object WS -Descending | Select-Object -First 1
    if ($game) {
        $mb = [math]::Round($game.WS/1MB, 0)
        ok "FiveM game child: $($game.ProcessName) pid=$($game.Id) ws=${mb}MB"
        if ($mb -lt 500) { wa "process ws=${mb}MB is small -- game may still be loading, wait for full in-game" }
    } else {
        wa "FiveM launcher running but no game subprocess yet -- WAIT until you see the game world"
        $issues += "game not loaded"
    }
}

hd "4. Is ollie.exe running?"
$ollie = Get-Process -Name ollie -ErrorAction SilentlyContinue
if ($ollie) { ok "ollie.exe running pid=$($ollie.Id)" }
else { wa "ollie.exe not running -- launch build\Release\ollie.exe" }

hd "5. scripthookv already loaded in GTA process?"
if ($game) {
    try {
        $mods = $game.Modules | Where-Object { $_.ModuleName -match 'scripthookv' }
        if ($mods) { ok "scripthookv.dll loaded in pid $($game.Id): $($mods[0].FileName)" }
        else { ko "scripthookv.dll NOT loaded in gtaprocess -- cheat will boot but natives OFF"; $issues += "shv not in process" }
    } catch { wa "cannot enumerate modules (ACL) -- run PowerShell as admin to see" }
}

hd "6. Payload log"
$log = "$env:TEMP\ollie.log"
if (Test-Path $log) {
    $ll = (Get-Item $log).LastWriteTime
    $age = [math]::Round(((Get-Date) - $ll).TotalSeconds, 0)
    ok "log at $log (updated ${age}s ago)"
    info "--- last 30 lines ---"
    Get-Content $log -Tail 30 | ForEach-Object { Write-Host "    $_" -ForegroundColor DarkGray }
    info "--- end ---"
    $lc = Get-Content $log -Raw
    if ($lc -match 'NATIVES LIVE')         { ok "SELF-TEST reports NATIVES LIVE -- natives working" }
    elseif ($lc -match 'natives OFF')       { ko "SELF-TEST reports natives OFF -- scripthookv didn't load"; $issues += "self-test off" }
    elseif ($lc -match 'boot\. built')      { wa "payload booted but didn't reach self-test" }
    else { ko "log has no boot line -- payload never injected" }
} else { wa "no log yet at $log -- payload never ran, or you haven't injected yet" }

hd "7. Verdict"
if ($issues.Count -eq 0) {
    Write-Host "  All green. If menu still misbehaves, send me the log + screenshot." -ForegroundColor Green
} else {
    Write-Host "  Problems identified:" -ForegroundColor Red
    foreach ($i in $issues) { Write-Host "    - $i" -ForegroundColor Red }
    Write-Host ""
    Write-Host "  Recovery steps:" -ForegroundColor Yellow
    if ($issues -contains "fivem not running")  { Write-Host "    1. Launch FiveM and join a server (reach the loading screen or in-game)" }
    if ($issues -contains "game not loaded")    { Write-Host "    2. Wait until the game window shows (not just the launcher)" }
    if ($issues -contains "no scripthookv")     { Write-Host "    3. Install ScriptHookV into FiveM.app (some servers block it)" }
    if ($issues -contains "shv not in process") { Write-Host "    3. Server may be blocking SHV. Try a non-anti-cheat server or singleplayer FiveM" }
    if ($issues -contains "self-test off")      { Write-Host "    4. Same as above: scripthookv not loaded in your current session" }
    if ($issues -contains "no exe")             { Write-Host "    5. Run run_build.ps1" }
    Write-Host "    6. Launch ollie.exe AFTER the game window is visible"
}

Write-Host "`n--- ollie diag end --- .gg/ojailbreaks ---" -ForegroundColor Magenta
