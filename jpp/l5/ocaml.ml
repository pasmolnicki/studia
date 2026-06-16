let rec binomial n k =
  if k = 0 || n = k then 1
  else binomial (n - 1) k + binomial (n - 1) (k - 1)

let binomial2 n k =
  let rec next_row row =
    let rec zip = function
      | [] | [_] -> [1]
      | x :: y :: tl -> (x + y) :: zip (y :: tl)
    in 1 :: zip row
  in
  let rec generate current_row i =
    if i = n then current_row
    else generate (next_row current_row) (i + 1)
  in
  let row = generate [1] 0 in
  List.nth row k

let rec mergesort l =
  let rec split l acc1 acc2 = match l with
    | [] -> (acc1, acc2)
    | [x] -> (x :: acc1, acc2)
    | x :: y :: tl -> split tl (x :: acc1) (y :: acc2)
  in
  let rec merge l1 l2 = match l1, l2 with
    | [], l -> l
    | l, [] -> l
    | x :: tl1, y :: tl2 ->
        if x <= y then x :: merge tl1 l2
        else y :: merge l1 tl2
  in
  match l with
  | [] -> []
  | [x] -> [x]
  | _ ->
      let l1, l2 = split l [] [] in
      merge (mergesort l1) (mergesort l2)

let rec de a b =
  if b = 0 then (1, 0, a)
  else
    let (x', y', z) = de b (a mod b) in
    (y', x' - (a / b) * y', z)

let prime_factors n =
  let rec factors d p =
    if d = 1 then []
    else if d mod p = 0 then p :: factors (d / p) p
    else if p * p > d then [d]
    else factors d (p + 1)
  in factors n 2

let totient n =
  let rec gcd a b = if b = 0 then a else gcd b (a mod b) in
  let rec count i acc =
    if i > n then acc
    else if gcd n i = 1 then count (i + 1) (acc + 1)
    else count (i + 1) acc
  in count 1 0

let totient2 n =
  let rec unique = function
    | [] -> []
    | x :: tl -> x :: unique (List.filter (fun y -> y <> x) tl)
  in
  let factors = unique (prime_factors n) in
  List.fold_left (fun acc p -> acc * (p - 1) / p) n factors

let primes n =
  let rec sieve = function
    | [] -> []
    | p :: tl -> p :: sieve (List.filter (fun x -> x mod p <> 0) tl)
  in
  let rec range i j = if i > j then [] else i :: range (i + 1) j in
  sieve (range 2 n)
