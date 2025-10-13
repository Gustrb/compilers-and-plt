-- not has a Bool -> Bool type
-- not True = False
-- not False = True 

-- (||) has a Bool -> Bool -> Bool type
-- So, it takes a || b and outputs a c, which must be of type Bool 
-- True || True = True
-- True || False = True
-- False || True = True
-- False || False = False

-- (&&) has a Bool -> Bool -> Bool type as well

-- Int has 64 bits
-- Integer is arbitrarly big, (bigint).
-- Relational operators are as following: <, >, <=, >=, ==, /=, (!= is not defined); 
-- For some reason, negative numbers are a bit weird, if we pass them to fuctions, we
-- need to wrap then in parenthesis, so for instance
-- even (-5) works, and returns false
-- even -5 gives the following error:
-- No instance for ‘Show (Integer -> Bool)’
-- arising from a use of ‘print’
-- (maybe you haven't applied a function to enough arguments?)
-- Other operations are:
-- div => integer division
-- mod => integer modulo
-- rem => remainder of the division

-- To represent real numbers, we use Float and Double (32 and 64 bit variants).
-- Literals, can be done like this: 3.14, 1e-9, -3.0, etc
-- we still have <, >, <=, >=, ==, /= operators as with the integer ones
-- We can use the fromIntegral type, to convert an integer into a double
-- and to convert from double(or float) to Integer we use round, floor, ceiling.

-- To represent characters, we use the Char type
-- It has the relational operators: <. >, <=, >=, ==, /=
-- And to do conversions it is necessary to: `import Data.Chat`
-- and then: 
-- ord :; Char -> Int
-- chr :: Int -> Char

-- Some usefull predefined functions:
-- even : Integral a => a -> Bool
-- odd : Integral a => a -> Bool

-- Minimum and maximum between 2 values
-- min :: Ord a => a -> a -> a
-- max :: Ord a => a -> a -> a

-- gcd
-- greatest common divisor
-- gcd :: Integral a => a -> a -> a 
-- least common multiple
-- lcm :: Integral a => a -> a -> a 

-- Math functions:
-- abs :: Num a => a -> a
-- sqrt :: Floating a => a -> a
-- log :: Floating a => a -> a
-- exp :: Floating a => a -> a
-- cos :: Floating a => a -> a


