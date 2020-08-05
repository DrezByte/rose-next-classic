param (
  [ValidateSet("release", "debug")]
  [string]$buildConfig = "release"
)

# Setup vswhere.exe
$vs_where = "C:/Program Files (x86)/Microsoft Visual Studio/Installer/vswhere.exe"
if (-not (Test-Path -Path $vs_where)) {
  Write-Error "Could not find vswhere. Is visual studio installer installed? Expected location: $vs_where"
  exit
}
Write-Host "Found vswhere: $vs_where"

# Find Visual Studio Installation path
$vs_version = "[16.3,17.0)"
$vs_install = & $vs_where -version $vs_version -property installationPath
if (-not (Test-Path -Path $vs_install)) {
  Write-Error "Could not find Visual Studio that matches version range $vs_version"
  exit
}
Write-Host "Found Visual Studio: $vs_install"

# Start Visual Studio Shell
Import-Module (Join-Path $vs_install "Common7\Tools\Microsoft.VisualStudio.DevShell.dll")
Enter-VsDevShell -VsInstallPath $vs_install -SkipAutomaticLocation
Write-Host "Started Visual Studio shell"

$rose_next_root = Join-Path $PSScriptRoot ..

# Build thirdparty
Write-Host "Building thirdparty"
& MSBuild.exe "/p:Configuration=$buildConfig;Platform=x86" $rose_next_root\thirdparty.sln
if (!$?) {
  exit 1
}

Write-Host "Building Rust projects"
Push-Location $rose_next_root/src

# Build Rust projects
$rust_toolchain = "stable-i686-pc-windows-msvc"
& rustup toolchain install $rust_toolchain
& rustup override set $rust_toolchain
Write-Host "Set Rust to use $rust_toolchain toolchain"

if ($buildConfig -eq "debug") {
  & cargo build
  if (!$?) {
    Pop-Location
    exit 1
  }
}
else {
  & cargo build --release
  if (!$?) {
    Pop-Location
    exit 1
  }
}
Pop-Location

# Build C++ projects
Write-Host "Building C++ projects"
& MSBuild.exe "/p:Configuration=$buildConfig;Platform=x86" $rose_next_root\rose-next.sln
if (!$?) {
  exit 1
}