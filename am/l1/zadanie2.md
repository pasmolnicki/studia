## Symulowane wyżarzanie

Symulowane wyżarzanie to jedna z technik projektowania algorytmów heurystycznych (metaheurystyka). Cechą charakterystyczną tej metody jest występowanie parametru sterującego zwanego temperaturą, który maleje w trakcie wykonywania algorytmu. Im wyższą wartość ma ten parametr, tym bardziej chaotyczne mogą być zmiany. Podejście to jest inspirowane zjawiskami obserwowanymi w metalurgii: im większa temperatura metalu, tym bardziej jest on plastyczny.

Jest to metoda iteracyjna: najpierw losowane jest pewne rozwiązanie, a następnie jest ono w kolejnych krokach modyfikowane. Jeśli w danym kroku uzyskamy rozwiązanie lepsze, wybieramy je zawsze. Istotną cechą symulowanego wyżarzania jest jednak to, że z pewnym prawdopodobieństwem może być również zaakceptowane rozwiązanie gorsze (ma to na celu umożliwienie wyjście z maksimum lokalnego).

Prawdopodobieństwo przyjęcia gorszego rozwiązania wyrażone jest wzorem e^((f(X)−f(X′))/T) (rozkład Boltzmanna), gdzie X jest poprzednim rozwiązaniem, X′ nowym rozwiązaniem, a f funkcją oceny jakości – im wyższa wartość f(X), tym lepsze rozwiązanie. Ze wzoru można zauważyć, że prawdopodobieństwo przyjęcia gorszego rozwiązania spada wraz ze spadkiem temperatury i wzrostem różnicy jakości obu rozwiązań.

Przez rozpoczęciem wykonywania algorytmu należy ustalić:
    - początkową wartość temperatury T;
    - sposób obniżania temperatury (często stosowanym rozwiązaniem jest mnożenie aktualnej temperatury przez pewien współczynnik, zazwyczaj mieszczący się w przedziale [0.8,0.99]);
    - liczbę prób przeprowadzanych w ramach jednej epoki (z tą samą temperaturą);
    - sposób wyboru nowego rozwiązania w ramach pojedynczej próby;
    - warunek stopu (może to być np. określona liczba epok lub brak lepszego rozwiązania w trakcie ostatnio wykonanych epok).
  
Działanie algorytmu można opisać następująco:
    1. Wylosuj rozwiązanie początkowe X.
    2. Wybierz losowe rozwiązanie X′ znajdujące się w sąsiedztwie X.
    3. Jeśli X′jest lepsze to je przyjmij (X := X′). W przeciwnym razie, wyznacz prawdopodobieństwo przyjęcia nowego rozwiązania używając wzoru e^((f(X)−f(X′))/T) i z tym prawdopodobieństwem X := X′.
    4. Jeśli nie wykonano jeszcze odpowiedniej liczby prób w obrębie danej epoki, wróć do punktu 2.
    5. Zmniejsz temperaturę.
    6. Jeśli nie osiągnięto jeszcze warunku stopu, wróć do punktu 2.

Ostatecznie, rezultatem działania algorytmu jest najlepsze znalezione w trakcie obliczeń rozwiązanie.

## Tabu Search

Tabu Search jest algorytmem który rozszerza Local Search przez dodanie mechanizmu zabronień (listy tabu), którego celem jest umożliwienie opuszczania ekstremów lokalnych oraz uniemożliwienie wpadania algorytmu w cykle. Za każdym razem kiedy rozwiązanie jest wybierane z otoczenia, jego reprezentacja jest umieszczana na liście tabu. Ponieważ zabronione jest wybieranie rozwiązania którego reprezentacja znajduje się na liście tabu, algorytm może opuścić ekstremum lokalne.

Przez rozpoczęciem wykonywania algorytmu należy ustalić:
    - długość listy tabu (można wykorzystać wyniki z poprzedniego zadania, czyli ustalić jakąś wielokrotność liczby poprawek prowadzących do lokalnego minimum);
    - sposób wyboru nowego rozwiązania w ramach pojedynczej próby (deterministyczny po wszystkich sąsiadach czy losowy po pewnej ich próbce);
    - warunek stopu (może to być np. koniec miejsca na liście lub brak poprawy w jakimś odcinku czasu).

Działanie algorytmu można opisać następująco:
  1. Wybierz rozwiązanie początkowe (np. wylosuj).
  2. Przeszukaj otoczenie bieżącego rozwiązania (deterministycznie lub losowo) i wybierz najlepsze którego nie ma na liście.
  3. Umieść je na liście i przyjmij jako bieżące rozwiązanie.
  4. Sprawdź warunek stopu i jeśli nie zachodzi to wróć do punktu 2 (warunkiem stopu może być np. koniec pojemności listy, lub liczba iteracji bez znalezienia nowego najlepszego rozwiązania).

Ostatecznie, rezultatem działania algorytmu jest najlepsze znalezione w trakcie obliczeń rozwiązanie.

# Zadania

Dla danych z poprzedniej listy oraz zbiorów Oman, Canada, Tanzania, Egypt i Ireland (mu1979.tsp, ca4663.tsp, tz6117.tsp, eg7146.tsp, ei8246.tsp) wykonaj następujące zadania:

### Zadanie 1

Zaimplementuj algorytm symulowanego wyżarzania i na małych danych (poniżej 1000
wierzchołków) przetestuj wybór parametrów. Opisz w sprawozdaniu w jaki sposób zostały wybrane te parametry. Następnie dla dużych danych wykonaj po 100 prób i podaj
najlepsze uzyskane rozwiązanie oraz średnią wartość uzyskanych rozwiązań.

### Zadanie 2

Zaimplementuj algorytm Tabu Search i na małych danych (poniżej 1000 wierzchołków)
przetestuj wybór parametrów. Opisz w sprawozdaniu w jaki sposób zostały wybrane
te parametry. Następnie dla dużych danych wykonaj po 100 prób i podaj najlepsze
uzyskane rozwiązanie oraz średnią wartość uzyskanych rozwiązań.


### Zadanie 3 

W języku Python, porównaj skuteczność i czas dotychczas użytych metod (z tej i poprzedniej listy).