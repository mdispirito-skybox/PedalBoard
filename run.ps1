<#
.SYNOPSIS
Launches the GuitarAmpSim application from the CMake build folder.

.DESCRIPTION
This script finds and executes the GuitarAmpSim.exe file based on the specified
configuration (Debug or Release).

.PARAMETER Configuration
Specifies the build configuration to run. Accepts 'Release' (default) or 'Debug'.

.EXAMPLE
# Run the optimized Release build (default)
.\run.ps1

.EXAMPLE
# Run the Debug build for debugging
.\run_app.ps1 -Configuration Debug
#>
param(
    [ValidateSet('Debug', 'Release')]
    [string]$Configuration = 'Release'
)

# 1. Define the base path for the build output
$ProjectRoot = $PSScriptRoot
$ExecutableName = "Big Mike's PedalBoard.exe"

# 2. Construct the full path to the executable
$ExecutablePath = Join-Path -Path $ProjectRoot -ChildPath "build\GuitarAmpSim_artefacts\$Configuration\$ExecutableName"

# 3. Check if the file exists
if (-not (Test-Path $ExecutablePath)) {
    Write-Error "Could not find the executable: $ExecutablePath"
    Write-Error "Please ensure you have built the '$Configuration' configuration in VS Code first."
    exit 1
}

# 4. Start the application
Write-Host "Starting [$Configuration] application from: $ExecutablePath"
Start-Process -FilePath $ExecutablePath