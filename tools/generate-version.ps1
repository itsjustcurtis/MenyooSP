<#
.SYNOPSIS
    Generates the C++ version files from the nearest reachable Git version tag.

.DESCRIPTION
    Exact tags produce their version without a suffix. Untagged commits append
    the number of commits since the tag. Stable tags increment the patch number
    and use an alpha prerelease suffix for development builds.

.EXAMPLE
    powershell -NoProfile -ExecutionPolicy Bypass -File .\tools\generate-version.ps1
#>
[CmdletBinding()]
param(
    [string]$OutputDirectory
)

$ErrorActionPreference = 'Stop'
$versionTagPattern = '^v(?<version>(0|[1-9][0-9]*)\.(0|[1-9][0-9]*)\.(0|[1-9][0-9]*)(?:-(?:0|[1-9][0-9]*|[0-9]*[A-Za-z-][0-9A-Za-z-]*)(?:\.(?:0|[1-9][0-9]*|[0-9]*[A-Za-z-][0-9A-Za-z-]*))*)?(?:\+[0-9A-Za-z-]+(?:\.[0-9A-Za-z-]+)*)?)$'

if ([string]::IsNullOrWhiteSpace($OutputDirectory)) {
    $OutputDirectory = Join-Path $PSScriptRoot '..\Solution\generated'
}

function Invoke-Git([string[]]$Arguments) {
    $result = & git @Arguments 2>&1
    if ($LASTEXITCODE -ne 0) {
        throw "Git failed: git $($Arguments -join ' ')`n$result"
    }
    return ($result | Out-String).Trim()
}

function Convert-VersionTag([string]$Tag) {
    if ($Tag -notmatch $versionTagPattern) {
        return $null
    }

    return $Matches.version
}

function Get-VersionTag {
    $candidateTags = (Invoke-Git @('tag', '--merged', 'HEAD', '--list', 'v[0-9]*')) -split "`r?`n" | Where-Object { $_ }
    $reachableTags = @()

    foreach ($tag in $candidateTags) {
        $tagVersion = Convert-VersionTag $tag
        if ($null -eq $tagVersion) {
            continue
        }

        $tagCommit = Invoke-Git @('rev-list', '-1', "$tag^{commit}")
        $height = [int](Invoke-Git @('rev-list', '--count', "$tagCommit..HEAD"))
        $reachableTags += [pscustomobject]@{
            Tag = $tag
            Version = $tagVersion
            Commit = $tagCommit
            Height = $height
        }
    }

    if ($reachableTags.Count -eq 0) {
        throw 'No reachable Git tag matching v<major>.<minor>.<patch>[-prerelease] was found.'
    }

    return $reachableTags | Sort-Object Height, Tag | Select-Object -First 1
}

function Get-GeneratedVersion($tag) {
    if ($tag.Height -eq 0) {
        return $tag.Version
    }

    if ($tag.Version -match '^(?<major>\d+)\.(?<minor>\d+)\.(?<patch>\d+)(?<prerelease>-[^+]+)?(?<build>\+.*)?$') {
        if ($Matches.prerelease) {
            return "$($Matches.major).$($Matches.minor).$($Matches.patch)$($Matches.prerelease).$($tag.Height)$($Matches.build)"
        }

        return "$($Matches.major).$($Matches.minor).$([int]$Matches.patch + 1)-alpha.$($tag.Height)$($Matches.build)"
    }

    throw "Could not parse version '$($tag.Version)'."
}

function Write-IfChanged([string]$Path, [string]$Content) {
    $existing = if (Test-Path -LiteralPath $Path) { [System.IO.File]::ReadAllText($Path) } else { $null }
    if ($existing -ne $Content) {
        [System.IO.File]::WriteAllText($Path, $Content, (New-Object System.Text.UTF8Encoding($false)))
    }
}

$tag = Get-VersionTag
$version = Get-GeneratedVersion $tag
$outputDirectory = [System.IO.Path]::GetFullPath($OutputDirectory)
New-Item -ItemType Directory -Force -Path $outputDirectory | Out-Null

$header = @"
#pragma once

#define MENYOO_CURRENT_VER_ "$version"
"@ -replace "`r?`n", "`r`n"

Write-IfChanged (Join-Path $outputDirectory 'Version.h') $header
Write-Output "Version $version ($($tag.Height) commits after $($tag.Tag))"
