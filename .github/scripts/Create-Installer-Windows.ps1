[CmdletBinding()]
param(
    [ValidateSet('x64', 'arm64')]
    [string] $Target = 'x64'
)

$ErrorActionPreference = 'Stop'

if ( $DebugPreference -eq 'Continue' ) {
    $VerbosePreference = 'Continue'
    $InformationPreference = 'Continue'
}

function CreateInstaller {
    trap {
        Write-Error $_
        exit 2
    }

    $ScriptHome = $PSScriptRoot
    $ProjectRoot = Resolve-Path -Path "$PSScriptRoot/../.."

    $UtilityFunctions = Get-ChildItem -Path $PSScriptRoot/utils.pwsh/*.ps1 -Recurse

    foreach( $Utility in $UtilityFunctions ) {
        Write-Debug "Loading $($Utility.FullName)"
        . $Utility.FullName
    }

    Install-BuildDependencies -WingetFile "${ScriptHome}/.Wingetfile"

    $GitDescription = Invoke-External git describe --tags --long
    $Tokens = ($GitDescription -split '-')
    $CommitVersion = $Tokens[0..$($Tokens.Count - 3)] -join '-'
    $CommitHash = $($Tokens[-1]).SubString(1)
    $CommitDistance = $Tokens[-2]

    if ( $CommitDistance -gt 0 ) {
        $OutputName = "vura-${CommitVersion}-${CommitHash}"
    } else {
        $OutputName = "vura-${CommitVersion}"
    }

    Log-Group "Creating Windows installer..."

    $InnoFile = "$ProjectRoot/installer/installer.iss"
    $InnoFileContent = Get-Content "$InnoFile"

    $InnoFileContent[1] = "#define MyAppVersion '${CommitVersion}'"
    $InnoFileContent[22] = "OutputBaseFilename=vura-${CommitVersion}-win64"

    $InnoFileContent | Set-Content "$InnoFile"
}

CreateInstaller