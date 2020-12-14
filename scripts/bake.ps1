param (
    [ValidateScript( { Test-Path $_ })]
    [string]$tools = (Join-Path $PSScriptRoot .. "bin" "release"),

    [ValidateScript( { Test-Path $_ })]
    [string]$manifest = (Join-Path $PSScriptRoot .. "bake.manifest"),

    [ValidateScript( { Test-Path $_ })]
    [string]$in = (Join-Path $PSScriptRoot .. "assets/"),

    [ValidateScript( { Test-Path $_ })]
    [string]$out = (Join-Path $PSScriptRoot .. "bin" "assets"),

    [string[]]$flags = @()
)

$pipeline = (Join-Path $tools "pipeline.exe")
& $pipeline bake -c $manifest $in $out @flags
