# TODO: Figure out how this fits into the workflow for the source tree?
# Perhaps our dev environment should be fully symlinked here?
# 	dev/game/ (symlink to the asset builds?)
# 	dev/server/ (symlink to the asset builds?)
# The main consideration is the client creates files that are not part of the
# disribution so we want to avoid polluting our build. With the server, we want
# to be able to support a server.toml
#
# @echo off
# set SERVER_DATA=%~dp0\..\server\data\3ddata

# IF NOT EXIST %SERVER_DATA% (
# 	mkdir %SERVER_DATA%\..
# 	mklink /J %SERVER_DATA% %~dp0..\game\3ddata
# ) ELSE (
# 	echo %SERVER_DATA% already exists
# )


# pause
function symlink_children {
    param (
        [ValidateScript( { Test-Path $_ })]
        [string]$src,

        [string]$dst
    )

    $children = Get-ChildItem -Path $src
    Foreach ($child in $children) {
        $leaf = Split-Path -Path $child -Leaf
        $out = (Join-Path $dst $leaf)
        if (Test-Path -Path $child -PathType Container) {
            New-Item -ItemType Junction -Path $out -Target $child
        }
        else {
            New-Item -ItemType HardLink -Path $out -Target $child
        }
    }
}

$root = (Join-Path $PSScriptRoot ..)
$asset_dir = (Join-Path $root "bin" "assets")

# Client links
$client_dir = (Join-Path $root "dev" "client")
symlink_children -src $asset_dir -dst $client_dir


# Server links
$server_dir = (Join-Path $root "dev" "server" "data")
$asset_3ddata = (Join-Path $asset_dir "3ddata")
$server_3ddata = (Join-Path $server_dir "3ddata")

New-Item -ItemType Junction -Path $server_3ddata -Target $asset_3ddata