param (
    [ValidateScript( { Test-Path $_ })]
    [string]$tools = (Join-Path $PSScriptRoot .. "tools/"),

    [ValidateScript( { Test-Path $_ })]
    [string]$manifest = (Join-Path $PSScriptRoot .. "pack.manifest"),

    [ValidateScript( { Test-Path $_ })]
    [string]$in = (Join-Path $PSScriptRoot .. "build"),

    [ValidateScript( { Test-Path $_ })]
    [string]$out = (Join-Path $PSScriptRoot .. "dist" "client"),

    [string[]]$flags = @()
)

$pipeline = (Join-Path $tools "pipeline.exe")
& $pipeline pack -c $manifest $in $out @flags
