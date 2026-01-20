Dodaj kolekcję reviews, która będzie przechowywac recenzje książek - referencję do ksiązki (po odpowiednim _id), dane recenzenta, ocenę w skali 1-5 oraz tekst
recenzji. Wstaw dane dotyczące co najmniej trzech recenzji dla jednej ksiązki (jedna z oceną ponizej 3 i jedna z oceną 5). Wstaw recezje kilku książek wystawione przez tego samego autora. Jaki wpływ na wstawianie/wyszukiwanie danyh ma przyjęta przez Ciebie metoda przechowywania informacji o recenzencie?
Dodaj dla kazdego dokumentu w kolekcji ˙ authors nowe pole: "awards" –
tablicę nagród (np. nazwa nagrody, rok otrzymania), z mozliwością pustej tablicy, jeśli autor nie otrzymał nagród. Dodaj nowe pole w kolekcji books: "genres" - tablica stringów reprezentująca gatunki literackie (np. "Fantasy", "Horror").
Napisz w sprawozdaniu raport z wykonanych czynnosci i odpowiedzi na pytania.

3. Wyszukiwanie danych, agregacja (10pkt)
• Wyszukaj wszystkie ksiązki napisane po polsku w gatunku "Fantasy" (1pkt)
• Za pomocą aggregate:
– wyszukaj wszystkie ksiązki, których średnia ocena w recenzjach to co najmniej 4 (1pkt)
– wyszukaj wszystkie ksiązki napisane przez autora o konkretnym imieniu
i nazwisku (1pkt)
– wyszukaj dane o ksiązkach napisanych przez polskich autorów wraz z naz-
wiskami tych autorów i srednią oceną książek (1pkt)
– wyswietl nazwisko autora oraz liczbę książek, które napisał (1pkt)
– policz srednią ocenę książek każdego autora (1pkt)
– znajdź autorów, którzy nie otrzymali zadnej nagrody (1pkt)
– policz ile ksiązek przypada na każdy gatunek literacki (1pkt)
– znajdź osobę, która napisała najwięcej recenzji (1pkt)
– policz srednią ocenę książek w zależności od języka (1pkt) 