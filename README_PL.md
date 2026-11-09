# RottingBass v0.2.0

Działające własne DSP wzmacniające bas przez generowanie harmonicznych. Brzmienie jest przybliżeniem celu, bez kalibracji do RBass. Produkt RottingBass, producent Rottingpitstufio. Własna grafika wektorowa. C++17 / JUCE 8.0.6.

- Freq: wydzielenie dolnego pasma 32–256 Hz.
- Intensity: poziom generowanych harmonicznych -24…+24 dB.
- Original Bass IN/OUT: zachowanie/usunięcie wydzielonego dołu.
- Gain: wyjście -48…+12 dB.

Mono/stereo, niezależne kanały, oversampling 4x, zgłaszanie latencji, wygładzanie automatyzacji 20 ms, odtwarzanie stanu. Zakresy i algorytm są własnym projektem, nie odwzorowaniem potwierdzonym pomiarami RBass. Po aktualizacji stare projekty odtworzą te same nastawy, ale ich brzmienie zmieni się względem szkieletu Gain-only.

## Windows x64 / MSVC
Wymagane Visual Studio 2022: Desktop development with C++ i Windows SDK; CMake >=3.22. Domyślnie pobierany jest przypięty JUCE 8.0.6 (SHA256 w CMake).
```
cmake -S . -B build -G "Visual Studio 17 2022" -A x64 -DRB_BUILD_TESTS=ON
cmake --build build --config Release --parallel 2 --target RottingBass_VST3 RottingBassSmoke RottingBassHostTest
build\RottingBassSmoke_artefacts\Release\RottingBassSmoke.exe
build\RottingBassHostTest_artefacts\Release\RottingBassHostTest.exe "%CD%\build\RottingBass_artefacts\Release\VST3\RottingBass.vst3"
```
Binarne wydanie jest gotowe dopiero po udanym Windows build, teście załadowania w hoście i opublikowaniu ZIP-a. Workflow .github/workflows/windows.yml sprawdza metadane i stabilne class ID, tworzy pełny bundle ZIP i publikuje release tylko po testach. W ZIP-ie znajdują się logi i BUILD_INFO.txt. Sam kod źródłowy nie dowodzi testu Windows.

Instalacja: docs/INSTALL_WINDOWS_PL.txt. REAPER nie został dotąd osobno zweryfikowany. Własny host testowy nie zastępuje testu w REAPER-ze.

## Tożsamość i archiwum
Zachowane z v0.1.0: manufacturer code Rtpt, plugin code RtBs, BUNDLE_ID studio.rottingpitstufio.rottingbass, JUCE_VST3_CAN_REPLACE_VST2=0, ID parametrów freq/intensity/originalBass/gain i version hint 1. Nie zmieniaj ich przy aktualizacjach. Poprzednia pełna paczka jest zachowana w archives/v0.1.0 (w repozytorium). Kolejnych buildów i release nie nadpisujemy.

## Pomiary
Nie otrzymano referencji RBass. docs/RENDER_PL.md opisuje pierwszą sesję: 66 Hz, +6.9 dB, -9.5 dB, IN i OUT. `python tools/generate.py` odtwarza wszystkie 54 WAV float32/48k; w repozytorium nie dublujemy WAV z archiwum v0.1.0. Narzędzia features/analyze/difference/abx zachowane. Testy narzędzi: `python -m unittest discover -s tests -v` po instalacji requirements.txt. Raport: docs/VALIDATION_PL.md.

Licencja własnego kodu: MIT (LICENSE). JUCE: AGPLv3 lub odpowiednia licencja JUCE. Żadnych zasobów ani kodu Waves. Użytkownik wybiera właściwą ścieżkę licencyjną dystrybucji JUCE.
