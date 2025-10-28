import Data.List
import Data.Char

numUniques :: (Eq a) => [a] -> Int
numUniques = length . nub

nub' :: (Eq a) => [a] -> [a]
nub' = foldl (\acc x -> if x `elem` acc then acc else acc ++ [x]) []

-- The tails function generates all
isIn :: (Eq a) => [a] -> [a] -> Bool
needle `isIn` haystack = any (needle `isPrefixOf`) (tails haystack)

encode :: Int -> String -> String
encode offset msg = map (\c -> chr $ ord c + offset) msg

decode :: Int -> String -> String
decode shift msg = encode (negate shift) msg 

digitSum :: Int -> Int
digitSum = sum . map digitToInt . show

findKey :: (Eq k) => k -> [(k, v)] -> Maybe v
findKey key = foldr (\(k, v) acc -> if key == k then Just v else acc) Nothing
