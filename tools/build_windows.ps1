$ErrorActionPreference = 'Stop'
$sourceRoot = Split-Path $PSScriptRoot -Parent
$releaseRoot = Join-Path $sourceRoot 'releases/v0.2.0'
if (Test-Path $releaseRoot) { throw "Release already exists: $releaseRoot. Preserve it and increment the version." }
$buildDir = Join-Path $sourceRoot 'build'
cmake -S $sourceRoot -B $buildDir -G 'Visual Studio 17 2022' -A x64 -DRB_BUILD_TESTS=ON
if ($LASTEXITCODE -ne 0) { throw 'MSVC configure failed' }
cmake --build $buildDir --config Release --parallel 2 --target RottingBass_VST3 RottingBassSmoke RottingBassHostTest
if ($LASTEXITCODE -ne 0) { throw 'MSVC build failed' }
& (Join-Path $buildDir 'RottingBassSmoke_artefacts/Release/RottingBassSmoke.exe') 2>&1 | Tee-Object (Join-Path $buildDir 'processor-test.log')
if ($LASTEXITCODE -ne 0) { throw 'Processor test failed' }
$bundle = Join-Path $buildDir 'RottingBass_artefacts/Release/VST3/RottingBass.vst3'
& (Join-Path $buildDir 'RottingBassHostTest_artefacts/Release/RottingBassHostTest.exe') $bundle 2>&1 | Tee-Object (Join-Path $buildDir 'windows-host-test.log')
if ($LASTEXITCODE -ne 0) { throw 'VST3 host test failed' }
$packageDir = Join-Path $releaseRoot 'windows-x64'
New-Item -ItemType Directory -Path $packageDir -Force | Out-Null
Copy-Item $bundle $packageDir -Recurse
Copy-Item (Join-Path $sourceRoot 'docs/INSTALL_WINDOWS_PL.txt') (Join-Path $packageDir 'INSTALL_PL.txt')
Copy-Item (Join-Path $buildDir 'processor-test.log'),(Join-Path $buildDir 'windows-host-test.log') $packageDir
Get-ChildItem $packageDir -File -Recurse | Get-FileHash -Algorithm SHA256 | ConvertTo-Json | Set-Content (Join-Path $packageDir 'SHA256.json')
$zip = Join-Path $releaseRoot 'RottingBass-v0.2.0-Windows-x64.zip'
Compress-Archive -Path "$packageDir/*" -DestinationPath $zip
Write-Output "Ready for Windows installation: $zip. REAPER verification remains separate."
