# Walidacja v0.2.0

Brzmienie: autorski algorytm, przybliżenie celu. Brak renderów RBass: nie wykonano kalibracji, testu null ani odsłuchów ABX. Nie deklarujemy zgodności próbka w próbkę ani spełnienia progu -60 dB. Narzędzia i niezależne sygnały walidacyjne z v0.1.0 zachowano.

Algorytm: dół wydzielony filtrem Butterwortha drugiego rzędu @Freq; nasycenie tanh(3*x), gałąź parzysta z prostowania i nieparzysta z reszty nasycenia. Wygenerowany sygnał ma filtr górnoprzepustowy 0.75*Freq i dolnoprzepustowy czwartego rzędu min(8*Freq,6000 Hz). Intensity jest gainem gałęzi harmonicznych, nie progiem kompresji. OUT odejmuje wydzielony dół od toru oryginalnego. Wszystko pracuje w 4x oversamplingu IIR; JUCE zgłasza całkowite opóźnienie do hosta. Wygładzanie parametrów 20 ms. Kanały są niezależne; brak limitera i normalizacji.

Testy automatyczne: wpływ każdego parametru na audio, prawidłowy gain, obecność drugiej harmonicznej, mono/stereo, 44.1/48/96 kHz, zapis/odczyt stanu, powtarzalność, izolacja kanałów, cisza, niezależność od bloku, przekroczenie deklarowanej długości bloku oraz automatyzacja. Osobny host ładuje skompilowany VST3 i uruchamia te same testy. Faktycznie wykonane testy dokumentują logi; sama obecność kodu testów nie oznacza ich przejścia.

REAPER: niezweryfikowany. Windows: patrz logi przy binarce wydania, nie log natywnego Linuxa. Po dostarczeniu referencji wykonamy identyfikację IN/OUT, harmonicznych/fazy, intermodulacji i obwiedni. Null będzie wyrównany wyłącznie o potwierdzone opóźnienie, bez korekcji gainu/EQ/fazy. Dotychczasowego holdoutu nie użyto do dostrajania.

Wykonano: Linux Release, test procesora i załadowanego VST3 w hoście JUCE — PASS (docs/build-evidence). Test bloku większego od deklaracji dotyczy samego procesora: host VST3 zawsze przestrzega maxSamplesPerBlock. Windows i REAPER wymagają osobnych dowodów; status Windows potwierdzi workflow.
