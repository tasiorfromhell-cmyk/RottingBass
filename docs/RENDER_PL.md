# Pomiary RBass — pierwsza sesja

Pliki inputs/fit są przeznaczone do dopasowania. inputs/validation pozostaw nierenderowane do zamrożenia modelu. Każdy plik: 48 kHz, float32, stereo, 6 s; 1 s ciszy przed sygnałem i 1 s po. Nie przycinaj ciszy.

1. Nowy projekt REAPER: 48 kHz, tempo dowolne, playback rate 1.0, brak stretchingu, track/master 0 dB, pan centralny, brak wysyłek i innych FX. Wyłącz automatyczne fade in/out na importowanych itemach. Sprawdź brak media/item/take gain i normalizacji.
2. Importuj każdy plik inputs/fit na osobną ścieżkę, WSZYSTKIE od 0.000 s. Nie ustawiaj kolejno w czasie. Sygnały stereo pozostaw stereo. Na każdej ścieżce umieść jedną identyczną instancję licencjonowanego stereo RBass.
3. Ustaw Freq 66 Hz, Intensity +6.9 dB, Gain -9.5 dB, Original Bass IN. Zweryfikuj odczyt numeryczny i brak automatyzacji. Zapisz projekt jako RBass_66_IN.rpp.
4. Zaznacz ścieżki; File > Render: Source = Stems (selected tracks), Bounds = Custom time range 0.000–6.000 s. Sample rate 48000, Channels Stereo, WAV 32 bit FP. Wyłącz normalize/limit, dither, noise shaping i render tail. Nazwa każdego renderu musi być identyczna jak jego wejścia (nazwij ścieżki bez rozszerzenia i użyj $track). Renderuj do renders/66_IN. Sprawdź jeden plik: dokładnie 288000 ramek.
5. Przełącz WSZYSTKIE instancje na Original Bass OUT, nie zmieniając innych nastaw. Zapisz jako RBass_66_OUT.rpp. Renderuj te same ścieżki do renders/66_OUT.
6. Renderuj ponownie kontrolny sinus 66 Hz i impuls do osobnego katalogu repeat, aby wykryć niedeterminizm. Zanotuj REAPER version, RBass version, Waves build, mono/stereo component, block size, host PDC, offline mode oraz screenshot nastaw w session.txt. Nie przesuwaj renderów ręcznie.
7. Prześlij dwa katalogi renderów, wejścia (jeżeli zmienione), session.txt i numer wersji RBass. Nie przesyłaj instalatora, licencji ani kodu Waves.

## Prawdziwa stopa i bas
W real/ umieść własne kick_fit.wav, bass_fit.wav oraz niezależne kick_validation.wav, bass_validation.wav. Przygotuj 48 kHz float32, zachowaj poziom źródła, bez normalizacji; dodaj 1 s ciszy przed i co najmniej 2 s po. Dla każdego zapisz długość, kanały i pochodzenie. Renderuj z tym samym początkiem i końcem jak wejście. Walidacyjne zachowaj do drugiego etapu. Nie zastępuj ich syntetycznymi próbkami.

## Kolejne sesje (po pierwszym dopasowaniu)
Siatka fit: Freq 40/66/100 Hz, Intensity 0/+6.9/+12 dB, Gain 0/-9.5, IN i OUT. Holdout: Freq 52/83 Hz, Intensity +3/+9 dB, Gain -4 dB. Potwierdź rzeczywiste zakresy w RBass; zakresy w szkielecie są tymczasowe. Powtórz generację --rate 44100 i --rate 96000 do osobnych katalogów, z identycznymi czasami, bez resamplingu hosta. Osobno sprawdź komponent mono oraz stereo L/R/LR/antyfaza. Zmierz automatykę i recall w REAPER, małe/duże bloki oraz zmiany parametrów podczas pracy.
