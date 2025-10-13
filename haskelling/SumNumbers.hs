sumNumbers :: Int -> Int
sumNumbers 0 = 0
sumNumbers n = n + sumNumbers(n-1)
