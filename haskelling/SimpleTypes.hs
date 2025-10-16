-- Int has a much smaller range than Integer...
-- This overflows for small values.
factorial :: Int -> Int
factorial n = product [1..n]

-- Integer has an "infinite" range, the underlying data structure is a
-- bignum
factorial' :: Integer -> Integer
factorial' n = product [1..n]


-- Float has half the precision of double
circumference :: Float -> Float
circumference r = 2 * pi * r

circumference' :: Double -> Double
circumference' r = 2 * pi * r

-- The succ function works on top of the enum type class
-- and it returns the next one in a sequence
add :: Integer -> Integer -> Integer
add 0 b = b
add a b = add (a-1) (succ b)

-- We can get the minimum of bounded types
-- using minBound :: Int
-- and the maximum maxBound :: Int 

