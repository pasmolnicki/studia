## Treść ogólna

Nie trzeba wykonywac wszystkich zadań, ani wszystkich ich części. W przypadku tego sprawozdania rekomendo-wane jest zrobić minimum zad. 2. (bez rozszerzonego algorytmu Euklidesa) i zad. 3., gdyż będą częściowo wykorzystywane w kolejnych sprawozdaniach.

## Twoja praca

Zadanie 1 zostało zrealizowane w języku Rust, jednie sprawdź czy jest poprawne i czy wyniki są zgodne z oczekiwaniami.

Zadanie 2 wypełnij jedynie podpunkt a), w języku C++

Zadanie 3 ukończ w całości, też w jęzku C++

## Dane

a = 2, b = 8, c = 3, d = 9, e = 9, f = 9

### Zadanie 1

Język: Rust

Zaimplementuj w wybranym języku pierscień liczb Gaussa Z[i]. Opisz jak reprezentowane są te liczby.
Zaimplementuj dla nich: normę, dzielenie z resztą, NWD i NWW. Opisz pokrótce jak wyliczane są te funkcje. Sprawozdanie tego zadania powinno zawierać:
- Fragmenty kodu dla dzielenia z resztą i NWD,
- Wywołanie dla Npa  biq oraz dzielenia (c + a) + (b + d)i przez e + fi. Podaj wszystkie możliwe wyniki i uzasadnij dlaczego nie ma ich więcej.
- Wywołanie NWD i NWW dla trójki liczb: a + bi, c + di, e + di (podaj wszystkie możliwe wyniki). Uwaga: dodaj implementację NWD i NWW wywoływanych dla listy liczb (niekoniecznie tylko dwóch liczb). Jak wygladają
wywołania dla listy pustej i 1-elementowej?

### Zadanie 2

Język: C++ (wersja 20+ z wykorzystaniem szablonów oraz konceptów)

Zaimplementuj w wybranym języku pierscień wielomianów R[x], w taki sposób, żeby można było łatwo go rozszerzyć do dowolnego κ[x_1, ..., x_n], gdzie κ jest ciałem. Opisz jak reprezentujesz te wielomiany i wyjaśnij krótko dlaczego. Dla R[x] zaimplementuj normę, dzielenie z resztą, NWD, NWW oraz rozszerzony algorytm wyznaczania
NWD oraz krótko opisz ich działanie (można odnosić się do rozwiązania zadania 1.) 
W sprawozdaniu zawrzyj:
- Wywołanie normy wielomianu cx^a + b oraz jego dzielenia przez wielomian x + 1.
- Wywołanie (rozszerzonego) NWD dla pary vpxq  ax3bx2cxd, wpxq  dx3ex2fx. Na tej podstawie
znajd´z taką stałą g, ze˙ 1 nie nalezy do NWD dla ˙ vpxq i wpxq  g. Wylicz NWWpvpxq, wpxq  gq.

### Zadanie 3

Zaimplementuj porządki produktowe <= na N^n, gdzie n jest liczbą naturalną oraz algorytm, który dla zadanej skończonej listy elementów zbioru A należącego do N^n zwraca elementy <=-minimalne w A i precyzyjnie opisz jego działanie. Porównaj ze sobą:
- pary: (a, b), (c, d), (e, f) (w porządku na N^2)
- trójki: (a, c, e), (b, d, f) (w porządku na N^3)
- Znajdź elementy minimalne w zbiorach: A = {(x, y) z N^2: (x - a)^2 + (y - b)^2 <= 5} oraz B = {(x, y, z, w) z N^4: (x - c)^2 + (y - d)^2 + (z - e)^2 + (w - f)^2 > 224}.