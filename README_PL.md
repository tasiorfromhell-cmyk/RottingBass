# RottingBass v0.1.0

Autorski szkielet C++/JUCE i paczka pomiarowa. UWAGA: to NIE jest jeszcze model RBass. Działa tylko Gain; Freq, Intensity i Original Bass oczekują na pomiary. Nazwa produktu RottingBass, producent Rottingpitstufio; własna grafika wektorowa rysowana w Source/Plugin.cpp.

## Windows x64
Wymagane Visual Studio 2022 (Desktop development with C++), CMake >=3.22, internet do pobrania JUCE 8.0.6. Uruchom w terminalu deweloperskim:
```
cmake -S . -B build -A x64
cmake --build build --config Release --target RottingBass_VST3
```
Wynik: build/RottingBass_artefacts/Release/VST3/RottingBass.vst3 (cały katalog bundle). Kopiuj cały bundle do C:\Program Files\Common Files\VST3\RottingBass.vst3. W REAPER przeskanuj VST3. Sprawdź mono/stereo, zapis/ponowne otwarcie projektu i automatykę czterech parametrów. Workflow Windows w .github/workflows przygotowuje osobny artefakt. W tej paczce stan faktycznie wykonanej kompilacji opisuje BUILD_STATUS.txt.

Stała tożsamość: COMPANY Rottingpitstufio, manufacturer code Rtpt, plugin code RtBs, bundle studio.rottingpitstufio.rottingbass, JUCE 8.0.6, JUCE_VST3_CAN_REPLACE_VST2=0. NIE zmieniaj tych pól ani ID parametrów między wydaniami. Przy pierwszym Windows build zapisz class ID z moduleinfo.json w raporcie i porównuj kolejne wydania. Wersję oraz paczki numeruj osobno; poprzednich nie nadpisuj.

## Pomiary
```
python -m venv .venv
.venv\Scripts\pip install -r requirements.txt
.venv\Scripts\python tools/generate.py
```
Gotowe wejścia znajdują się w measurements/inputs. Instrukcja: docs/RENDER_PL.md. Najpierw wyrenderuj tylko fit przy 66 Hz / +6.9 dB / -9.5 dB, IN i OUT.
```
python tools/difference.py measurements/renders/66_IN measurements/renders/66_OUT measurements/difference
python tools/analyze.py measurements/manifest.json REF CANDIDATE --confirmed-delay 0 --delay-evidence "potwierdzony pomiar impulsu" --output report.json
python tools/abx.py REF.wav CANDIDATE.wav abx/trial01
```
Użyj delay=0 dopiero po pomiarze. Dostarczone narzędzia to podstawa analizy, nie automatyczny identyfikator RBass. Dalsza analiza intermodulacji, obwiedni i stereo wymaga referencji. Raport bieżący: docs/VALIDATION_PL.md.

Licencja własnego kodu: MIT (LICENSE). JUCE jest zależnością na warunkach JUCE/AGPL lub licencji komercyjnej; przed dystrybucją wybierz właściwą ścieżkę licencyjną. Nie dołączono kodu ani zasobów Waves. Repozytorium wskazane przez użytkownika było niedostępne (Git 403, connector 404); paczka nie została do niego wysłana.

Pomiary surowe harmonicznych/intermodulacji, obwiedni i kanałów:
```
python tools/features.py measurements/manifest.json measurements measurements/renders/66_IN --output features_IN.json
```
Powtórz dla OUT oraz różnicy IN−OUT. Hipotezę latencji z korelacji trzeba potwierdzić na impulsach i kolejnych sygnałach. Automatyczne testy narzędzi: `python -m unittest discover -s tests -v`. Opcjonalny test procesora: skonfiguruj `-DRB_BUILD_TESTS=ON`, zbuduj i uruchom RottingBassSmoke.

Alternatywnie uruchom `powershell -ExecutionPolicy Bypass -File tools/build_windows.ps1`. Skrypt buduje i testuje procesor, a następnie kopiuje bundle do releases/v0.1.0/windows-x64. Odmawia nadpisania wcześniejszej paczki. Flaga ExecutionPolicy dotyczy tego pojedynczego uruchomienia skryptu.
