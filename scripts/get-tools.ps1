# Download the latest tools from gitlab.
# Requires GITLAB_TOKEN to be set or passed as argument.
# Argument overrides env var.
param (
    [string]$token = $env:GITLAB_TOKEN,

    [ValidateScript( { Test-Path $_ })]
    [string]$out = (Join-Path $PSScriptRoot .. "tools/")
)

if (!$token) {
    Write-Error("Please set GITLAB_TOKEN or provide a token using '-token'");
    exit 1
}

If (!(test-path $out)) {
    New-Item -ItemType Directory -Force -Path $out
}

$base_url = "https://gitlab.com/api/v4/projects"
$project_id = "11940511"
$artifacts_url = "$base_url/$project_id/jobs/artifacts/master/raw"
$job_name = "build"

$pipeline_url = "$artifacts_url/bin/release/pipeline.exe?job=$job_name"
$texconv_url = "$artifacts_url/thirdparty/directxtex-2020.9.30/texconv.exe?job=$job_name"

$pipeline_out = (Join-Path $out "pipeline.exe")
$texconv_out = (Join-Path $out "texconv.exe")

& curl --location --header "PRIVATE-TOKEN: $token" $pipeline_url --output $pipeline_out
& curl --location --header "PRIVATE-TOKEN: $token" $texconv_url --output $texconv_out

