-- Functions in Haskell are *pure*. Pure functions are functions that only return results calculated
-- relative to their parameters.
-- Pure functions don't have side effects:
-- => They don't modify their parameters;
-- => They don't modify the memory;
-- => They don't modify input/output;

-- Pure functions can be thought as functions that always return the same result given the same parameters.
-- Function names **ALWAYS** start with upper case.
-- To declare a function, we first declare its type
-- then the definition, using formal parameters

-- Some function examples:
double :: Int -> Int
double x = 2 * x

perimeter :: Int -> Int -> Int
perimeter width height = double (width + height)

xOr :: Bool -> Bool -> Bool
xOr a b = (a || b) && not (a && b)

factorial :: Integer -> Integer
factorial n = if n == 0 then 1 else n * factorial (n - 1)

-- We could've defined the factorial function as follows
-- The language tries to match the patterns from top-to-bottom
-- so it checks if n is 0, then use the 0 branch
-- If it is not 0, the first one won't match, so it will try against the second one
-- which is a catch-all clause, so it executes

-- factorial :: Integer -> Integer
-- factorial 0 = 1 
-- factorial n = n * factorial (n - 1)

-- IMPORTANT
-- If this was the other way around, this would result in an infinite recursion
-- because n would be smaller than 0 and it would keep expanding further
-- factorial :: Integer -> Integer
-- factorial n = n * factorial (n - 1)
-- factorial 0 = 1 

-- We can also have catch all patterns, using _, when the parameter is not used
nand :: Bool -> Bool -> Bool
nand True True = False
nand _ _ = True

-- We can also use guards to conditionally execute code, this can be done as follows:
-- in this case, if the argument is negative we go to the first branch, otherwise, we execute the
-- catch all
valAbs :: Int -> Int
valAbs n
  | n >= 0 = n
  | otherwise = -n

fastExp :: Integer -> Integer -> Integer
fastExp _ 0 = 1
-- We use the let-in binding to define a binding that is going to be used
-- in the scope
fastExp x n =
  let y        = fastExp x n_halved
      n_halved = div n 2
  in
    if even n
    then y * y
    else y * y * x

-- The same function couldv've been declared the following way
-- fastExp :: Integer -> Integer -> Integer
-- fastExp _ 0 = 1
-- fastExp x n
--  | even n = y * y
--  | otherwise = y * y * x
--  where
--    y = fastExp x n_halved
--    n_halved = div n 2

-- There is a really important concept in haskell, that I wasn't really talking about before
-- that is currying.
-- In haskell all functions take only a single parameter. Functions that take
-- more than one parameter actually return a new function.
-- Example:
-- prod 3 5 is actually (prod 3) 5
-- So first we apply 3 in prod, that result a partially applicated function that takes on argument, which is
-- in this case, 5
-- Example
-- prod :: Int -> Int -> Int
-- Should be read as:
-- prod :: Int -> (Int -> Int)
-- And when we do (prod 3)
-- the result is (prod 3) :: (Int -> Int)
-- And when we do (prod 3) 5 :: Int

addVectors :: (Double, Double) -> (Double, Double) -> (Double, Double)
addVectors a b = (fst a + fst b, snd a + snd b)

addVectors' :: (Double, Double) -> (Double, Double) -> (Double, Double)
addVectors' (x1, y1) (x2, y2) = (x1 + x2, y1 + y2)

-- We have the built-in fst and snd to get the first and second elements
-- of a tuple, but what if we want more?
first :: (a, b, c) -> a
first (x, _, _) = x

second :: (a, b, c) -> b
second (_, x, _) = x

third :: (a, b, c) -> c
third (_, _, x) = x

head' :: [a] -> a
head' [] = error "Can't call head on an empty list, dummy!" 
head' (x:_) = x 

sum' :: [Integer] -> Integer 
sum' [] = 0 
sum' (x:xs) = x + sum' xs

firstLetter :: String -> String
firstLetter "" = "Empty string whoopsie!"
firstLetter all@(x:xs) = "The first letter of " ++ all ++ " is " ++ [x]

bmiTell :: Double -> String
bmiTell bmi
  | bmi <= 18.5 = "You are underweight, you emo, you!"
  | bmi <= 25.0 = "You are supposedly normal..."
  | bmi <= 30.0 = "You are fat! Lose some weight!"
  | otherwise = "You are a whale, congrats"

max' :: (Ord a) => a -> a -> a 
max' a b
  | a <= b = b 
  | otherwise = a

myCompare :: (Ord a) => a -> a -> Ordering
a `myCompare` b
  | a == b = EQ
  | a <= b = LT
  | otherwise = GT

bmiTell' weight height
  | bmi <= 18.5 = "You are underweight, you emo, you!"
  | bmi <= 25.0 = "You are supposedly normal..."
  | bmi <= 30.0 = "You are fat! Lose some weight!"
  | otherwise = "You are a whale, congrats"
  where bmi = weight / height^2


