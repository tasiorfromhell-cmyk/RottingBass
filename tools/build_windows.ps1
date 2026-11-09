$ErrorActionPreference = 'Stop'
$sourceRoot = Split-Path $PSScriptRoot -Parent
$destination = Join-Path $sourceRoot 'releases/v0.2.0/windows-x64'
if (Test-Path $destination) { throw "Build already exists: $destination. Preserve it and increment the release version." }
$buildDir = Join-Path $sourceRoot 'build'
cmake -S $sourceRoot -B $buildDir -A x64 -DRB_BUILD_TESTS=ON
if ($LASTEXITCODE -ne 0) { throw 'Configure failed' }
cmake --build $buildDir --config Release --target RottingBass_VST3 RottingBassSmoke RottingBassHostTest
if ($LASTEXITCODE -ne 0) { throw 'Build failed' }
& (Join-Path $buildDir 'RottingBassSmoke_artefacts/Release/RottingBassSmoke.exe')
if ($LASTEXITCODE -ne 0) { throw 'Processor test failed' }
& (Join-Path $buildDir 'RottingBassHostTest_artefacts/Release/RottingBassHostTest.exe') (Join-Path $buildDir 'RottingBass_artefacts/Release/VST3/RottingBass.vst3')
if ($LASTEXITCODE -ne 0) { throw 'VST3 host test failed' }
New-Item -ItemType Directory -Path $destination -Force | Out-Null
Copy-Item (Join-Path $buildDir 'RottingBass_artefacts/Release/VST3/RottingBass.vst3') $destination -Recurse
Get-ChildItem $destination -File -Recurse | Get-FileHash -Algorithm SHA256 | ConvertTo-Json | Set-Content (Join-Path $destination 'SHA256.json')
Write-Output "Ready: $destination. Verify in REAPER before release."
