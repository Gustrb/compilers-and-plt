chain :: Integer -> [Integer]
chain 1 = [1]
chain n
  | even n = n : chain (n `div` 2)
  | odd n  = n : chain (n * 3 + 1)

numLongChains :: Int -> Int
-- Get all the chains from 1 to 100 such that the length
-- is greater than 15
numLongChains n = length (filter isLong (map chain [1..100]))
  where isLong xs = length xs > n 

numLongChains' :: Int -> Int
numLongChains' n = length (filter (\xs -> length xs > n) (map chain [1..100]))

-- Finds the smallest number such that numLongChains == 0
minChainLength = head (filter (isSmallerThan) ([1..]))
  where isSmallerThan x = (numLongChains x) == 0

