# Raport v0.1.0 — oczekuje na referencje

Nie otrzymano renderów RBass. Nie wykonano identyfikacji, dopasowania DSP ani walidacji brzmienia. Próg -60 dB NIE jest potwierdzony. Nie deklarujemy zgodności próbka w próbkę. Szkielet przetwarza wyłącznie Gain (wygładzanie 20 ms); pozostałe parametry są zapisywane/automatyzowane, lecz jeszcze nie wpływają na dźwięk.

Plan identyfikacji: sprawdzić powtarzalność, ciszę, gain i latency na impulsach (korelacja jako hipoteza, potwierdzenie na wielu przebiegach); porównać IN−OUT i jego widmo/fazę z wejściem. Nie zakładać, że różnica jest idealnym filtrem dolnoprzepustowym. Pomiar harmonicznych: koherentne okno 2–4 s, amplitudy zespolone per kanał/fundamentalna; analizować zależność od poziomu. Dwuton: linie m*f1+n*f2 i ich fazy; atak/wybrzmienie: obwiednie i czas powrotu. Stereo: przesłuch i zależność LR od samych L/R. Nieliniowość może uniemożliwić wyznaczenie latencji z pojedynczego szczytu.

analyze.py liczy null bez dopasowania gainu, EQ i fazy. Wymaga jawnie potwierdzonego opóźnienia i opisu dowodu; porównuje tylko wspólny zakres po przesunięciu, podaje jego długość. Raport zawiera każdy plik, peak reszty dBFS, RMS reszty/ref, harmoniczne i listę największych odchyleń. null w polach dB oznacza amplitudę zero; dla cichej referencji względny RMS jest nieokreślony. Brak pliku oznacza niepełną walidację, a nie sukces. Różnice harmonicznych poniżej progu referencji 1e-10 nie są oceniane.

ABX: abx.py tworzy A/B/X z wyrównaniem RMS oraz wspólnym zapasem szczytowym. To osobny odsłuch: nie używaj tych plików do null. Klucz PRIVATE ukryj przed słuchaczem. Zbieraj z góry ustaloną liczbę prób i wynik binomialny; samo nierozróżnianie nie dowodzi tożsamości.

Następna iteracja powinna wskazać największy błąd per plik/ustawienie, zmienić model tylko na fit i ponowić walidację. Po użyciu holdoutu do dostrajania nie jest już niezależny — trzeba przygotować nowy.
