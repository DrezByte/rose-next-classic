param (
    [ValidateSet("release", "debug")]
    [string]$config = "release",
    [switch]$client = $false,
    [switch]$server = $false,
    [switch]$tools = $false,
    [switch]$database = $false,
    [string]$out = (Join-Path $PSScriptRoot .. "dist/" $config)
)

$rose_next_root = Join-Path $PSScriptRoot ..
$build_root = Join-Path $rose_next_root bin $config
$doc_root = Join-Path $rose_next_root doc

$client_out = Join-Path $out client/
$server_out = Join-Path $out server/
$tool_out = Join-Path $out tool/
$db_out = Join-Path $out database/

$directxtex_ver = "2020.2.15"
$texconv = Join-Path $rose_next_root thirdparty directxtex-$directxtex_ver texconv.exe

$squash_script = Join-Path $PSScriptRoot "squash-migrations.ps1"

Write-Host $texconv

Write-Host "Collecting files to $out"

if ($client -eq $true) {
    Write-Host "Collecting client files"
    xcopy /Y (Join-Path $build_root triggervfs.dll) $client_out
    xcopy /Y (Join-Path $build_root rosenext.exe) $client_out
    xcopy /Y (Join-Path $build_root znzin.dll) $client_out
}
else {
    Write-Host "Skipping client files"
}

if ($server -eq $true) {
    Write-Host "Collecting server files"
    xcopy /Y (Join-Path $build_root sho_loginserver.exe) $server_out
    xcopy /Y (Join-Path $build_root sho_worldserver.exe) $server_out
    xcopy /Y (Join-Path $build_root sho_gameserver.exe) $server_out
    xcopy /Y (Join-Path $doc_root server.toml.example) $server_out
}
else {
    Write-Host "Skipping server files"
}

if ($tools -eq $true) {
    Write-Host "Collecting tools"
    xcopy /Y (Join-Path $build_root pipeline.exe) $tool_out
    xcopy /Y $texconv $tool_out
}
else {
    Write-Host "Skipping tools"
}

if ($database -eq $true) {
    Write-Host "Squashing database migrations"
    If (!(test-path $db_out)) {
        New-Item -ItemType Directory -Force -Path $db_out
    }

    & $squash_script -out $db_out
}
else {
    Write-Host "Skipping database"
}