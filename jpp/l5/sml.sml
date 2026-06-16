fun binomial n k =
    if k = 0 orelse n = k then 1
    else binomial (n - 1) k + binomial (n - 1) (k - 1)

fun binomial2 n k =
    let
        fun next_row [] = [1]
          | next_row [_] = [1]
          | next_row (x :: y :: tl) = (x + y) :: next_row (y :: tl)
        fun gen current_row i =
            if i = n then current_row
            else gen (1 :: next_row current_row) (i + 1)
        val row = gen [1] 0
    in
        List.nth (row, k)
    end

fun mergesort L =
    let
        fun split [] (a1, a2) = (a1, a2)
          | split [x] (a1, a2) = (x :: a1, a2)
          | split (x :: y :: tl) (a1, a2) = split tl (x :: a1, y :: a2)

        fun merge ([], l2) = l2
          | merge (l1, []) = l1
          | merge (x :: tl1, y :: tl2) =
            if x <= y then x :: merge (tl1, y :: tl2)
            else y :: merge (x :: tl1, tl2)
    in
        case L of
            [] => []
          | [x] => [x]
          | _ => let val (l1, l2) = split L ([], [])
                 in merge (mergesort l1, mergesort l2)
                 end
    end

fun de a b =
    if b = 0 then (1, 0, a)
    else
        let val (x', y', z) = de b (a mod b)
        in (y', x' - (a div b) * y', z)
        end

fun prime_factors n =
    let
        fun factors d p =
            if d = 1 then []
            else if d mod p = 0 then p :: factors (d div p) p
            else if p * p > d then [d]
            else factors d (p + 1)
    in
        factors n 2
    end

fun totient n =
    let
        fun gcd a b = if b = 0 then a else gcd b (a mod b)
        fun count i acc =
            if i > n then acc
            else if gcd n i = 1 then count (i + 1) (acc + 1)
            else count (i + 1) acc
    in
        count 1 0
    end

fun totient2 n =
    let
        fun filter p [] = []
          | filter p (x :: xs) = if x = p then filter p xs else x :: filter p xs
        fun unique [] = []
          | unique (x :: xs) = x :: unique (filter x xs)
        val factors = unique (prime_factors n)
    in
        List.foldl (fn (p, acc) => acc * (p - 1) div p) n factors
    end

fun primes n =
    let
        fun filter p [] = []
          | filter p (x :: xs) = if x mod p = 0 then filter p xs else x :: filter p xs
        fun sieve [] = []
          | sieve (p :: tl) = p :: sieve (filter p tl)
        fun range i j = if i > j then [] else i :: range (i + 1) j
    in
        sieve (range 2 n)
    end
