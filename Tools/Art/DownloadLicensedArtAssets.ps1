# ALSAEQA - Licensed Art Acquisition
# Unreal Engine 5.8 / Windows
#
# Downloads the official free/commercial-friendly asset packs from their authors' pages.
# The packs are NOT redistributed by this repository. Run this script locally, then import
# the downloaded GLB/FBX/OBJ assets into Content/ArtSource and commit only files whose
# licenses permit repository distribution.
#
# Sources checked 2026-09-03:
# Quaternius Universal Base Characters: https://quaternius.com/packs/universalbasecharacters.html
# Quaternius Universal Animation Library: https://quaternius.com/packs/universalanimationlibrary.html
# Quaternius Universal Animation Library 2: https://quaternius.com/packs/universalanimationlibrary2.html
# Quaternius Fantasy Props MegaKit: https://quaternius.com/packs/fantasypropsmegakit.html
# Quaternius Medieval Village MegaKit: https://quaternius.com/packs/medievalvillagemegakit.html
# Quaternius Stylized Nature MegaKit: https://quaternius.com/packs/stylizednaturemegakit.html
#
# IMPORTANT: itch.io may require an interactive download flow. Therefore this script opens
# the official download pages rather than guessing private CDN URLs or bypassing access controls.

$ErrorActionPreference = 'Stop'
$Root = Resolve-Path (Join-Path $PSScriptRoot '..\..')
$Drop = Join-Path $Root 'Content\ArtSource'
New-Item -ItemType Directory -Force -Path $Drop | Out-Null

$Sources = @(
    @{ Name='UniversalBaseCharacters'; Url='https://quaternius.itch.io/universal-base-characters/purchase' },
    @{ Name='UniversalAnimationLibrary'; Url='https://quaternius.itch.io/universal-animation-library/purchase' },
    @{ Name='UniversalAnimationLibrary2'; Url='https://quaternius.itch.io/universal-animation-library-2/purchase' },
    @{ Name='FantasyPropsMegaKit'; Url='https://quaternius.itch.io/fantasy-props-megakit' },
    @{ Name='MedievalVillageMegaKit'; Url='https://quaternius.itch.io/medieval-village-megakit' },
    @{ Name='StylizedNatureMegaKit'; Url='https://quaternius.itch.io/stylized-nature-megakit' }
)

Write-Host 'ALSAEQA licensed art acquisition' -ForegroundColor Cyan
Write-Host "Destination: $Drop"
Write-Host ''

foreach ($Source in $Sources) {
    Write-Host "[$($Source.Name)] $($Source.Url)" -ForegroundColor Yellow
    Start-Process $Source.Url
}

Write-Host ''
Write-Host 'Download the Standard/free packages from the official pages that opened.' -ForegroundColor Green
Write-Host 'Place the archives in Content/ArtSource/Incoming and extract them there.'
Write-Host 'Then follow Docs/REAL_ART_ASSET_PIPELINE.md for Unreal import and retargeting.'
