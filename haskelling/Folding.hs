sum' :: (Num a) => [a] -> a 
-- Foldl takes a function that is going to be ran on every element
-- an initial element, and the list that is going to be processed
-- sum' xs = foldl (\acc x -> acc + x) 0 xs
sum' = foldl (+) 0

-- This is the same as doing it like so
-- sum' [] = 0
-- sum' (x:xs) = x + sum' xs


-- Left fold starts from the left, and right fold from the end of the list
-- so we can implement map using right folds like so
map' :: (a -> b) -> [a] -> [b]
map' f xs = foldr (\x acc -> f x : acc) [] xs

-- This could also be implemented using a foldl but that would involve adding to the end of the list, which is much slower than to the beggining
-- map' f xs = foldl (\acc x -> acc ++ [f x]) [] xs



