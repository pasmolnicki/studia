Poniżej pełny przewodnik — najpierw tekst, potem kilka interaktywnych diagramów ilustrujących kluczowe mechanizmy.

---

## Prolog dla programisty imperatywnego

### 1. Zmiana paradygmatu

W Rust/C++/Pythonie piszesz **jak** zrobić coś krok po kroku. W Prologu opisujesz **co** jest prawdą, a interpreter sam szuka dowodu.

```python
# Python — algorytm: powiedz komputerowi co ma robić
def max(a, b):
    if a > b: return a
    else: return b
```

```prolog
% Prolog — relacja: opisz kiedy coś jest prawdą
max(A, B, A) :- A >= B.
max(A, B, B) :- B > A.
```

Prolog to silnik wnioskowania logicznego. Pytasz go: *„czy istnieje X takie że P(X) jest prawdą?"* — on sam przeszukuje bazę wiedzy.

---

### 2. Terminy — jedyna struktura danych

Wszystko w Prologu jest **termem**. Cztery rodzaje:

| Rodzaj | Przykłady | Reguła rozpoznania |
|---|---|---|
| **Atom** | `hello`, `foo`, `'Jan Kowalski'` | małą literą lub w apostrofach |
| **Liczba** | `42`, `3.14`, `-7` | cyfra na początku |
| **Zmienna** | `X`, `Lista`, `_Tmp`, `_` | WIELKĄ literą lub `_` |
| **Struktura** | `point(3,4)`, `f(X,g(Y))` | `funktor(arg1, arg2, ...)` |

Listy to syntaktyczny cukier na struktury: `[1,2,3]` = `'.'(1,'.'(2,'.'(3,'[]')))`. Pisane jako `[Głowa|Ogon]`.

**Zmienna `_` (anonimowa)** — gdy wynik Cię nie interesuje. Każde wystąpienie `_` to osobna, niezwiązana zmienna. Dwie różne `_` w tej samej klauzuli to dwie różne zmienne.

---

### 3. Baza wiedzy: fakty i reguły

Program Prologu to zbiór **klauzul** (ang. *clauses*). Są dwa rodzaje:

```prolog
% FAKT — bezwarunkowo prawdziwy
rodzic(tom, bob).       % "tom jest rodzicem boba"
rodzic(bob, ann).

% REGUŁA — prawdziwa jeśli spełnione są warunki po :-
dziadek(X, Z) :-        % "X jest dziadkiem Z"
    rodzic(X, Y),       % jeśli X jest rodzicem Y
    rodzic(Y, Z).       % i Y jest rodzicem Z
```

Składnia reguły: `Głowa :- Ciało.`

Ciało to lista celów oddzielonych przecinkami (= AND) lub średnikami (= OR):

```prolog
% Przecinek = AND
dorosly_mezczyzna(X) :- mezczyzna(X), wiek(X, W), W >= 18.

% Średnik = OR (rzadko używany, preferowane osobne klauzule)
pojazd(X) :- samochod(X) ; rower(X).
```

Każda klauzula kończy się **kropką** — to separator, nie terminator zdania.

---

### 4. Unifikacja — serce Prologu

Unifikacja (`=`) to próba **dopasowania** dwóch termów przez podstawienie wartości za zmienne. To NIE jest porównanie (`==`) ani przypisanie.

```prolog
?- X = 42.           % X staje się 42           → true, X=42
?- foo(X,3) = foo(1,Y). % X=1, Y=3             → true
?- X = Y, Y = hello. % X=Y=hello               → true
?- 1 = 2.            % nie da się dopasować     → false
```

**Kluczowa różnica od przypisania:**

```prolog
?- X = 1, X = 2.   % BŁĄD — X jest już 1, nie można zmienić na 2 → false
```

Zmienna w Prologu to nie "pudełko na wartość" — to **nazwa miejsca w termie**. Raz ujednolicona (ang. *instantiated*), jest związana na stałe w ramach danej gałęzi obliczeń.

---

### 5. Zapytania i jak Prolog szuka odpowiedzi

Zapytanie (`?-`) to cel który Prolog próbuje udowodnić. Szuka go **od góry do dołu** przez klauzule, **od lewej do prawej** przez cele w ciele.

```prolog
rodzic(tom, bob).
rodzic(bob, ann).
rodzic(bob, pat).

dziadek(X, Z) :- rodzic(X, Y), rodzic(Y, Z).

?- dziadek(tom, Kto).
```

Prolog pyta: *"czy istnieje Kto takie że dziadek(tom, Kto) jest prawdą?"*

---

### 6. Nawracanie (Backtracking)

To mechanizm który odróżnia Prolog od wszystkiego innego. Gdy cel się nie powiedzie, Prolog **cofa się** do ostatniego punktu wyboru i próbuje alternatywnej klauzuli.

Analogia z C++: wyobraź sobie że kompilator automatycznie dodaje `goto` do każdego miejsca gdzie był wybór, gdy bieżąca ścieżka prowadzi do fałszu.---

### 7. Listy

Lista to albo `[]` (pusta), albo para `[Głowa|Ogon]` gdzie Ogon jest listą. Identycznie jak `cons` w Lisp czy `(head, tail)` w Haskellu.

```prolog
[1, 2, 3]          % cukier syntaktyczny na:
[1 | [2 | [3 | []]]]

% Wzorce dopasowania
[H|T] = [1,2,3]    % H=1, T=[2,3]
[A,B|R] = [a,b,c]  % A=a, B=b, R=[c]
[X] = [42]         % X=42  (lista jednoelementowa)
[_|T] = [a,b,c]    % T=[b,c], pierwszego elementu nie potrzebujesz
```

Typowy predykat listowy ma dwie klauzule: przypadek bazowy (lista pusta) i krok rekurencyjny:

```prolog
% member(+Elem, +Lista) — czy Elem jest na Liście?
member(X, [X|_]).           % X jest głową → gotowe
member(X, [_|T]) :-         % X nie jest głową → szukaj w ogonie
    member(X, T).

% length(+Lista, -Dlugosc)
my_length([], 0).
my_length([_|T], N) :-
    my_length(T, N1),
    N is N1 + 1.
```

**Pułapka dla imperatywistów:** nie ma pętli `for`/`while`. Iteracja = rekurencja. Prolog jest optymalny dla rekurencji ogonowej (TCO), analogicznie do Rusta.

---

### 8. Arytmetyka — operator `is`

Prolog **nie oblicza** wyrażeń automatycznie. `X = 2+3` daje `X = 2+3` (strukturę), nie `5`. Do obliczenia służy `is`:

```prolog
?- X is 2 + 3.      % X = 5        ✓
?- X = 2 + 3.       % X = 2+3      (struktura, nie liczba!)
?- 5 is 2 + 3.      % true         ✓
?- X is 2 + Y.      % BŁĄD — Y musi być związane przed is
```

Operatory arytmetyczne to zwykłe funktory — `2+3` to term `+(2,3)`. Operator `is` **ewaluuje** prawy argument.

Porównania arytmetyczne (obie strony muszą być związane):

```prolog
=:=   % równość arytmetyczna:   3 =:= 3.0  → true
=\=   % nierówność arytmetyczna
<, >, =<, >=
```

Odróżnij od unifikacji `=` (strukturalnej) i `\=` (brak unifikacji).

---

### 9. Przepływ sterowania: Cut i Negacja

**Cut (`!`)** — zatrzymuje nawracanie w bieżącej klauzuli i eliminuje alternatywy dla predykatu. Jak `break` ale dla nawracania:

```prolog
max(X, Y, X) :- X >= Y, !.   % jeśli X >= Y: wynik X, koniec
max(_, Y, Y).                  % wpp: wynik Y
```

Bez `!` Prolog po znalezieniu pierwszego rozwiązania próbowałby drugiej klauzuli (co byłoby błędem dla X=Y).

**Negacja przez niepowodzenie (`\+`)** — `\+ Cel` jest prawdziwe gdy `Cel` nie może być udowodnione:

```prolog
nie_jest_ptakiem(X) :- \+ ptak(X).
```

To nie jest logiczna negacja — to *closed world assumption*: jeśli Prolog nie może udowodnić P, zakłada ¬P.

---

### 10. Czytanie napisanego kodu — analiza predykatów

Teraz możemy przeczytać napisany wcześniej kod.

**Mergesort — `split/3`:**

```prolog
split([], [], []).          % lista pusta → obie połowy puste
split([X], [X], []).        % singleton → do lewej
split([X,Y|T], [X|L], [Y|R]) :-   % 2+ elementy:
    split(T, L, R).         % X do lewej, Y do prawej, rekurencja na ogonie
```

Wzorzec `[X,Y|T]` dopasowuje listę z co najmniej dwoma elementami na raz. Prolog jednocześnie wiąże X, Y i T. Nie ma żadnej pętli — rekurencja idzie od 2 do 2 elementów na raz.

**Extended GCD — `extended_gcd/5`:**

```prolog
extended_gcd(A, 0, A, 1, 0).          % przypadek bazowy: gcd(A,0)=A, x=1, y=0
extended_gcd(A, B, GCD, X, Y) :-
    B > 0,
    Q is A // B,   % dzielenie całkowite
    R is A mod B,
    extended_gcd(B, R, GCD, X1, Y1),  % rekurencja
    X is Y1,                           % wzór Bézouta
    Y is X1 - Q * Y1.
```

`is` pojawia się wszędzie tam gdzie potrzeba obliczeń. `//` to dzielenie całkowite, `mod` to reszta.

**Prime factors — `prime_factors_aux/3`:**

```prolog
prime_factors_aux(1, _, []).           % 1 nie ma czynników
prime_factors_aux(N, F, [N]) :-        % F² > N → N jest pierwsze
    N > 1, F * F > N.
prime_factors_aux(N, F, [F|Fs]) :-    % F dzieli N → F jest czynnikiem
    N > 1, F * F =< N,
    N mod F =:= 0,
    N1 is N // F,
    prime_factors_aux(N1, F, Fs).     % nie zwiększaj F — może dzielić dalej
prime_factors_aux(N, F, Fs) :-        % F nie dzieli → następny kandydat
    N > 1, F * F =< N,
    N mod F =\= 0,
    F1 is F + 1,
    prime_factors_aux(N, F1, Fs).
```

Cztery klauzule = cztery przypadki. Prolog próbuje je **po kolei od góry**. `[F|Fs]` buduje listę czynników podczas zwijania stosu rekurencyjnego — analogicznie do Rust gdzie zwracasz `vec` przez `push` w pętli, ale tutaj lista „rośnie" w trakcie powrotu z rekurencji.

**Totient — `compute_totient/3`:**

```prolog
compute_totient(T, [], T).            % brak więcej czynników → wynik = N
compute_totient(N, [P|Ps], T) :-
    N1 is N // P * (P - 1),          % N * (P-1)/P (dzielenie dokładne)
    compute_totient(N1, Ps, T).
```

`N // P * (P-1)` to **lewostronne** działanie: najpierw `N // P`, potem `* (P-1)`. Działa poprawnie bo P dzieli N dokładnie w tym kroku.

**Sito — `sieve/2` i `remove_multiples/3`:**

```prolog
sieve([], []).
sieve([H|T], [H|Primes]) :-          % H jest pierwsze (nie odfiltrowane)
    remove_multiples(H, T, Remaining),
    sieve(Remaining, Primes).         % sito na reszcie

remove_multiples(_, [], []).
remove_multiples(P, [H|T], R) :-
    H mod P =:= 0,                    % wielokrotność → pomiń (R = wynik bez H)
    remove_multiples(P, T, R).
remove_multiples(P, [H|T], [H|R]) :-
    H mod P =\= 0,                    % nie wielokrotność → zostaje
    remove_multiples(P, T, R).
```

Dwie ostatnie klauzule `remove_multiples` są **wzajemnie wykluczające** przez warunki `=:= 0` i `=\= 0`. Prolog i tak spróbuje obu po kolei — druga klauzula po prostu zawsze zawiedzie gdy pierwsza się powiedzie, i na odwrót. To idiom Prologu: warunki w ciele zastępują `if/else`.

------

### 11. Pułapki dla imperatywistów

Kilka rzeczy które boli najbardziej na początku:

**Brak destrukcyjnego przypisania.** Nie istnieje coś jak `X = X + 1`. Zmienne są jednokierunkowe. Wzorzec akumulatora zastępuje pętle ze zmienną mutable:

```prolog
% Źle — niemożliwe
sum([], S, S).        % to jest OK (wzorzec akumulatora)
sum([H|T], Acc, S) :-
    Acc1 is Acc + H,  % NOWA zmienna Acc1, nie nadpisujesz Acc
    sum(T, Acc1, S).

?- sum([1,2,3], 0, S).   % S = 6
```

**Kolejność klauzul ma znaczenie.** Prolog próbuje je od góry. Przypadek bazowy rekurencji musi być PRZED krokiem rekurencyjnym jeśli chcesz ograniczyć nawracanie (albo użyj `!`).

**Arytmetyka wymaga `is`.** Zawsze. `X = 2+3` to struktura, `X is 2+3` to obliczenie.

**`=` to unifikacja, nie porównanie.** Do porównania liczbowego używaj `=:=`, do porównania termów `==`.

**Predykaty mogą być wielokierunkowe.** Jeden predykat może działać jako funkcja w wielu kierunkach jeśli jest poprawnie napisany:

```prolog
?- append([1,2], [3,4], X).   % X = [1,2,3,4]
?- append([1,2], Y, [1,2,3]). % Y = [3]
?- append(X, Y, [1,2,3]).     % wszystkie podziały listy!
```

To jest to czego nie ma w żadnym innym popularnym języku.
