[CmdletBinding()]
param(
    [string]$ReleaseTag = $(if ($env:ALSAEQA_ASSET_RELEASE_TAG) { $env:ALSAEQA_ASSET_RELEASE_TAG } else { 'assets-v1' }),
    [string]$ManifestPath = $(Join-Path $PSScriptRoot 'assets-manifest.json'),
    [string]$ProjectRoot = $(Split-Path -Parent (Split-Path -Parent $PSScriptRoot))
)

$ErrorActionPreference = 'Stop'

function Get-Sha256([string]$Path) {
    return (Get-FileHash -Algorithm SHA256 -LiteralPath $Path).Hash.ToLowerInvariant()
}

if (-not (Test-Path -LiteralPath $ManifestPath)) { throw "Assets manifest not found: $ManifestPath" }
$manifest = Get-Content -LiteralPath $ManifestPath -Raw | ConvertFrom-Json
if ($manifest.project -ne 'ALSAEQA') { throw 'Invalid ALSAEQA assets manifest.' }

$repo = if ($env:GITHUB_REPOSITORY) { $env:GITHUB_REPOSITORY } else { 'ali12hhh-oss/alsaeqa' }
$token = $env:GITHUB_TOKEN
if (-not $token) { throw 'GITHUB_TOKEN is required to download the authenticated GitHub Release asset.' }

$headers = @{
    Authorization = "Bearer $token"
    Accept = 'application/vnd.github+json'
    'X-GitHub-Api-Version' = '2022-11-28'
    'User-Agent' = 'ALSAEQA-Asset-Downloader'
}

$releaseUrl = "https://api.github.com/repos/$repo/releases/tags/$ReleaseTag"
Write-Host "Resolving ALSAEQA asset release '$ReleaseTag'..."
try {
    $release = Invoke-RestMethod -Uri $releaseUrl -Headers $headers -Method Get
} catch {
    throw "Asset release '$ReleaseTag' was not found or is not accessible. Publish the release and upload the required assets first. Details: $($_.Exception.Message)"
}

$downloadRoot = Join-Path $env:RUNNER_TEMP "alsaeqa-assets-$ReleaseTag"
if (Test-Path -LiteralPath $downloadRoot) { Remove-Item -LiteralPath $downloadRoot -Recurse -Force }
New-Item -ItemType Directory -Path $downloadRoot -Force | Out-Null
$allSourceRoots = @()

$assetsByName = @{}
foreach ($asset in $release.assets) { $assetsByName[$asset.name] = $asset }

foreach ($item in $manifest.assets) {
    if (-not $assetsByName.ContainsKey($item.file)) { throw "Required asset '$($item.file)' is missing from release '$ReleaseTag'." }

    $asset = $assetsByName[$item.file]
    $archivePath = Join-Path $downloadRoot $asset.name
    Write-Host "Downloading $($asset.name) ($($asset.size) bytes)..."
    Invoke-WebRequest -Uri $asset.url -Headers ($headers + @{ Accept = 'application/octet-stream' }) -OutFile $archivePath

    if ($item.sha256 -and $item.sha256 -ne 'REPLACE_AFTER_UPLOAD') {
        $actual = Get-Sha256 $archivePath
        if ($actual -ne $item.sha256.ToLowerInvariant()) { throw "SHA-256 mismatch for '$($asset.name)'. Expected $($item.sha256), got $actual." }
    } else {
        Write-Warning "SHA-256 is not pinned yet for '$($asset.name)'."
    }

    $destination = Join-Path $ProjectRoot $item.destination
    if (Test-Path -LiteralPath $destination) { Remove-Item -LiteralPath $destination -Recurse -Force }
    New-Item -ItemType Directory -Path $destination -Force | Out-Null

    $itemExtractRoot = Join-Path $downloadRoot ([IO.Path]::GetFileNameWithoutExtension([IO.Path]::GetFileNameWithoutExtension($asset.name)))
    if (Test-Path -LiteralPath $itemExtractRoot) { Remove-Item -LiteralPath $itemExtractRoot -Recurse -Force }
    New-Item -ItemType Directory -Path $itemExtractRoot -Force | Out-Null
    Write-Host "Extracting $($asset.name)..."
    Expand-Archive -LiteralPath $archivePath -DestinationPath $itemExtractRoot -Force

    # Expand nested ZIP packs inside THIS release item only.
    $expandedArchives = @{}
    $expanded = $true
    while ($expanded) {
        $expanded = $false
        $nested = @(Get-ChildItem -LiteralPath $itemExtractRoot -Recurse -File -Filter '*.zip' -ErrorAction SilentlyContinue)
        foreach ($nestedZip in $nested) {
            $key = $nestedZip.FullName.ToLowerInvariant()
            if ($expandedArchives.ContainsKey($key)) { continue }
            $expandedArchives[$key] = $true

            $nestedTarget = Join-Path $nestedZip.DirectoryName ([IO.Path]::GetFileNameWithoutExtension($nestedZip.Name))
            if (Test-Path -LiteralPath $nestedTarget) { Remove-Item -LiteralPath $nestedTarget -Recurse -Force }
            New-Item -ItemType Directory -Path $nestedTarget -Force | Out-Null
            Write-Host "Expanding nested source pack: $($nestedZip.Name)"
            Expand-Archive -LiteralPath $nestedZip.FullName -DestinationPath $nestedTarget -Force
            $expanded = $true
        }
    }

    $sourceRoot = $itemExtractRoot
    $entries = @(Get-ChildItem -LiteralPath $itemExtractRoot -Force)
    if ($entries.Count -eq 1 -and $entries[0].PSIsContainer -and $entries[0].Name -eq 'ALSAEQA_REAL_ASSETS') {
        $sourceRoot = $entries[0].FullName
    }

    $allSourceRoots += $sourceRoot
    Write-Host "Authored source root: $sourceRoot"
}

$sourceRootsPath = Join-Path $downloadRoot 'source-roots.json'
$allSourceRoots | ConvertTo-Json -Compress | Out-File -FilePath $sourceRootsPath -Encoding utf8
"ALSAEQA_ASSET_SOURCE_ROOTS=$sourceRootsPath" | Out-File -FilePath $env:GITHUB_ENV -Append

$contentRoot = Join-Path $ProjectRoot 'Content'
$uassetCount = @(Get-ChildItem -LiteralPath $contentRoot -Recurse -File -Filter '*.uasset' -ErrorAction SilentlyContinue).Count
$umapCount = @(Get-ChildItem -LiteralPath $contentRoot -Recurse -File -Filter '*.umap' -ErrorAction SilentlyContinue).Count
Write-Host "Pre-existing Unreal assets: $uassetCount .uasset, $umapCount .umap"
Write-Host "Real authored source assets are ready for the Unreal import stage from release '$ReleaseTag'."
