% ============================================================
%  PROLOG — ALGORYTMY MATEMATYCZNE
%  Kompilacja: SWI-Prolog (swipl prolog_algorithms.pl)
%  Testy:      ?- [prolog_algorithms], run_tests.
% ============================================================

:- use_module(library(lists)).   % numlist/3, sort/2

mergesort([], []).
mergesort([X], [X]).
mergesort(List, Sorted) :-
    List = [_,_|_],                          % przynajmniej 2 elementy
    split(List, Left, Right),
    mergesort(Left,  SortedLeft),
    mergesort(Right, SortedRight),
    merge_sorted(SortedLeft, SortedRight, Sorted).

split([], [], []).
split([X], [X], []).
split([X, Y | T], [X | L], [Y | R]) :-
    split(T, L, R).

% merge_sorted(+L1, +L2, -Merged)
% Scala dwie posortowane listy w jedną posortowaną.
merge_sorted([], R, R).
merge_sorted(L, [], L).
merge_sorted([H1|T1], [H2|T2], [H1|M]) :-
    H1 =< H2,
    merge_sorted(T1, [H2|T2], M).
merge_sorted([H1|T1], [H2|T2], [H2|M]) :-
    H1 > H2,
    merge_sorted([H1|T1], T2, M).

de(A, B, X, Y, Z) :-
    extended_gcd(A, B, Z, X, Y).

extended_gcd(A, 0, A, 1, 0).
extended_gcd(A, B, GCD, X, Y) :-
    B > 0,
    Q  is A // B,
    R  is A mod B,
    extended_gcd(B, R, GCD, X1, Y1),
    X  is Y1,
    Y  is X1 - Q * Y1.

prime_factors(N, Factors) :-
    N > 1,
    prime_factors_aux(N, 2, Factors).

prime_factors_aux(1, _, []).
prime_factors_aux(N, F, [N]) :-      % N jest pierwsze
    N > 1,
    F * F > N.
prime_factors_aux(N, F, [F|Fs]) :-   % F dzieli N
    N > 1,
    F * F =< N,
    N mod F =:= 0,
    N1 is N // F,
    prime_factors_aux(N1, F, Fs).
prime_factors_aux(N, F, Fs) :-        % F nie dzieli N → następny kandydat
    N > 1,
    F * F =< N,
    N mod F =\= 0,
    F1 is F + 1,
    prime_factors_aux(N, F1, Fs).


totient(1, 1).
totient(N, T) :-
    N > 1,
    prime_factors(N, Factors),
    sort(Factors, UniqueFactors),      % usuwa duplikaty
    compute_totient(N, UniqueFactors, T).

compute_totient(T, [], T).
compute_totient(N, [P|Ps], T) :-
    N1 is N // P * (P - 1),
    compute_totient(N1, Ps, T).

primes(N, []) :- N < 2.
primes(N, Primes) :-
    N >= 2,
    numlist(2, N, List),
    sieve(List, Primes).

sieve([], []).
sieve([H|T], [H|Primes]) :-
    remove_multiples(H, T, Remaining),
    sieve(Remaining, Primes).

remove_multiples(_, [], []).
remove_multiples(P, [H|T], R) :-
    H mod P =:= 0,                     % wielokrotność → pomiń
    remove_multiples(P, T, R).
remove_multiples(P, [H|T], [H|R]) :-
    H mod P =\= 0,                     % nie jest wielokrotnością → zachowaj
    remove_multiples(P, T, R).
