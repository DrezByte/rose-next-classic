param (
    [Parameter(Mandatory = $true)]
    [ValidateScript( { Test-Path $_ })]
    [string]$assets_dir,

    [switch]$client = $false,
    [string]$client_out = (Join-Path $PSScriptRoot .. "dist/client"),
    [switch]$server = $false,
    [string]$server_out = (Join-Path $PSScriptRoot .. "dist/server/data")
)

if ($client) {
    # TODO: Call pipeline bake, pack, etc.
}

if ($server) {
    Write-Host "Copy asset files for server to $server_out"

    Write-Host "$assets_dir\3ddata\npc\*.chr" "$server_out\3ddata\npc\"
    xcopy "$assets_dir\3ddata\npc\*.chr" "$server_out\3ddata\npc\" /s /Y /d
    xcopy "$assets_dir\3ddata\npc\*.zsc" "$server_out\3ddata\npc\" /s /Y /d
    xcopy "$assets_dir\3ddata\npc\*.zmo" "$server_out\3ddata\npc\" /s /Y /d
    xcopy "$assets_dir\3ddata\mdata\*.zmo" "$server_out\3ddata\mdata\" /s /Y /d
    xcopy "$assets_dir\3ddata\pat\*.zmo" "$server_out\3ddata\pat\" /s /Y /d
    xcopy "$assets_dir\3ddata\questdata\*.qsd" "$server_out\3ddata\questdata\" /s /Y /d
    xcopy "$assets_dir\3ddata\questdata\*.stb" "$server_out\3ddata\questdata\" /s /Y /d
    xcopy "$assets_dir\3ddata\stb\*.stb" "$server_out\3ddata\stb\" /s /Y /d
    xcopy "$assets_dir\3ddata\motion\*.zmo" "$server_out\3ddata\motion\" /s /Y /d
    xcopy "$assets_dir\3ddata\maps\*.him" "$server_out\3ddata\maps\" /s /Y /d
    xcopy "$assets_dir\3ddata\maps\*.mov" "$server_out\3ddata\maps\" /s /Y /d
    xcopy "$assets_dir\3ddata\maps\*.ifo" "$server_out\3ddata\maps\" /s /Y /d
    xcopy "$assets_dir\3ddata\maps\*.zon" "$server_out\3ddata\maps\" /s /Y /d
    xcopy "$assets_dir\3ddata\effect\*.zmo" "$server_out\3ddata\effect\" /s /Y /d
    xcopy "$assets_dir\3ddata\ai\*.aip" "$server_out\3ddata\ai\" /s /Y /d
    xcopy "$assets_dir\3ddata\ai\*.stb" "$server_out\3ddata\ai\" /s /Y /d
    xcopy "$assets_dir\3ddata\stb\*.stl" "$server_out\language\" /s /Y /d
    xcopy "$assets_dir\3ddata\stb\list_quest_s.stl" "$server_out\3ddata\questdata\" /s /Y /d
}