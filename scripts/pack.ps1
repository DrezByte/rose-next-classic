param (
    [ValidateScript( { Test-Path $_ })]
    [string]$tools = (Join-Path $PSScriptRoot .. "bin" "release"),

    [ValidateScript( { Test-Path $_ })]
    [string]$manifest = (Join-Path $PSScriptRoot .. "pack.manifest"),

    [ValidateScript( { Test-Path $_ })]
    [string]$in = (Join-Path $PSScriptRoot .. "bin" "assets"),

    [ValidateScript( { Test-Path $_ })]
    [string]$out = (Join-Path $PSScriptRoot .. "dist" "debug" "client"),

    [string[]]$flags = @()
)

$pipeline = (Join-Path $tools "pipeline.exe")
& $pipeline pack -c $manifest $in $out @flags
