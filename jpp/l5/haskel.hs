unique :: Eq a => [a] -> [a]
unique [] = []
unique (x:xs) = x : unique (filter (/= x) xs)

binomial :: Integer -> Integer -> Integer
binomial n k
  | k == 0 || n == k = 1
  | otherwise        = binomial (n-1) k + binomial (n-1) (k-1)

binomial2 :: Int -> Int -> Integer
binomial2 n k = (pascal !! n) !! k
  where
    pascal :: [[Integer]]
    pascal = iterate (\row -> zipWith (+) ([0] ++ row) (row ++ [0])) [1]

mergesort :: Ord a => [a] -> [a]
mergesort [] = []
mergesort [x] = [x]
mergesort xs = merge (mergesort ys) (mergesort zs)
  where
    (ys, zs) = splitAt (length xs `div` 2) xs
    merge [] ms = ms
    merge ms [] = ms
    merge (m:ms) (n:ns)
      | m <= n    = m : merge ms (n:ns)
      | otherwise = n : merge (m:ms) ns

de :: Integer -> Integer -> (Integer, Integer, Integer)
de a 0 = (1, 0, a)
de a b =
  let (x', y', z) = de b (a `mod` b)
  in (y', x' - (a `div` b) * y', z)

prime_factors :: Integer -> [Integer]
prime_factors n = factors n 2
  where
    factors 1 _ = []
    factors d p
      | d `mod` p == 0 = p : factors (d `div` p) p
      | p * p > d      = [d]
      | otherwise      = factors d (p + 1)

totient :: Integer -> Integer
totient n = fromIntegral $ length [x | x <- [1..n], gcd n x == 1]

totient2 :: Integer -> Integer
totient2 n = foldl (\acc p -> acc * (p - 1) `div` p) n (unique (prime_factors n))

primes :: Integer -> [Integer]
primes n = sieve [2..n]
  where
    sieve [] = []
    sieve (p:xs) = p : sieve [x | x <- xs, x `mod` p /= 0]
