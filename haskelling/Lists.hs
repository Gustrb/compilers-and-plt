somenumbers = [1, 2, 3, 4, 5, 6]

-- Lists are homogenous data structures in haskell
-- i.e. they can store only a single data type.

-- Here are a few operations one can do to a list
-- Concatenation:
-- We use the ++ operator, so will have all the digits
concatenated = somenumbers ++ [7, 8, 9, 0]

-- The cool thing is that, in haskell
-- strings are lists of char, so we can use ++
-- to concat strings as well
name = "gustavo"
lastname = "bauer"

fullname = name ++ " " ++ lastname

-- Important: appending to the end of a list is somewhat expensive, since we have to go through the whole list.
-- There is also an operation to add an element to the beggining of a list
phrase1 = 'A' : " small cat"

-- An interesting thing is that declaring lists like [...], is just a sugar
-- for sequential prepending i.e. 1:2:3:[]
-- So in this case, we first prepend 3
-- 1:2:[3]
-- then 2
-- 1:[2,3]
-- then 1
-- [1,2,3]

-- To get an element at an specific index (0-based) we use the !! operator

-- To compare two lists, we can use <=, >=, < and >
-- The comparing goes as following, lets say we have two lists, [3,4,2] and [3,4,3]
-- If we compare them. [3,4,2] < [3,4,3]
-- It will first compare the 3, then compare the 4, then compare the 2 with the 3
-- 2 < 3 therefore the first list is smaller.
-- Also, a nonempty list is always greater than an empty one

-- There is also a couple operator such as
-- head:
-- head fullname => 'g'
-- so head gets the first element of the list

-- tail:
-- tail fullname => "ustavo bauer"
-- so tail removes the head of the list and returns the rest

-- last
-- last fullname => 'r'
-- last returns the last element of a list

-- init:
-- init fullname => "gustavo baue"
-- returns everything but the last element of the list

-- length:
-- length fullname => 13
-- returns the length of the list

-- null:
-- null fullname => False
-- null [] => True
-- Returns wheter the list is empty

-- reverse:
-- reverse fullname => "reuab ovatsug"
-- reverses the elements of the list

-- take:
-- take 3 fullname => "gus"
-- take takes the first n elements of the list

-- drop
-- drop 3 fullname => "tavo bauer"
-- drops the first n elements of the list and returns the rest

-- maximum:
-- maximum concatenated => 9
-- returns the largest element

-- minimum:
-- minimum concatenated => 0
-- returns the smallest element

-- sum:
-- sum concatenated => 45
-- sums over all the elements of the list

-- product:
-- product concatenated => 0
-- Gets the full product over all elements

-- elem
-- 'g' `elem` fullname => True
-- Returns wheter the lhs is contained in the rhs
-- It is usually used as an infix operator

-- In haskell you can build ranges using the [m..n] syntax such that m <= n.
-- Here are a few examples of ranges:
-- [1..20]    => [1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20]
-- ['a'..'z'] => "abcdefghijklmnopqrstuvwxyz"
-- You can also specify a step, for instance:
-- [2,4..20] => will get all the even numbers <= 20
-- Those ranges with steps are pretty, dumb, for instance  [1,2,4,8,16..100]
-- does not work.

-- Lets say we want to get the first 24 multiples of 13, we can do it like this:
-- [13,26..24*13]
-- But we can be even smarter, by using an infinite list:
-- take 24 [13,26..]
-- Since haskell is lazy, this won't blow the computer's memory

-- Here are a few functions that can be used to generate long or infinite lists:
-- cycle:
-- takes a list and replicates it indefinitely
-- take 11 (cycle [1..10]) => [1,2,3,4,5,6,7,8,9,10,1]

-- repeat:
-- generates an infinite list out of repeating the given elemnt
-- take 2 (repeat 5) => [5,5]

-- replicate:
-- an easier way to create a list of a single item
-- replicate 3 10 => [10,10,10]

-- We can also use list comprehensions to be able
-- to build list mathematically 
-- “{ 2 · x|x ∈ N, x ≤ 10}”
firstEvens = [x*2 | x <- [1..10]]

-- We can also provide a predicate, so that this will
-- only generate an element if the predicate is true
firstEvensWithPredicate = [x*2 | x <- [1..10], x*2<=12]

boomBangs xs = [if x < 10 then "BOOM!" else "BANG!" | x <- xs, odd x]

-- We can draw values from multiple sources. When it is the case,
-- all the permutations are generated
cartesianProduct s t = [(x, y) | x <- s, y <- t]

possibleAdjectivesAndNouns adj n = [adjective ++ " " ++ noun | adjective <- adj, noun <- n]

length' xs = sum [1 | _ <- xs]

removeNonUppercase st = [c | c <- st, c `elem` ['A'..'Z']]

-- Haskell supports tuples, which are ordered, and are heterogeneous
-- we declare tuples using (elem1, elem2)
-- We can use the fst function to get the first value of a tuple
first t = fst t
second t = snd t

-- zip joins two list into matching pairs
zipit a b = zip a b

