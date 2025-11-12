# Alphabet

The alphabet is a set of symbols, which can be used to represent words.
Usually it is denoted by the Greek symbol sigma.

For example, this is the alphabet of Latin letters:
Σ = {A, ..., Z}

And this is the one of binary numbers:
Σ = {0, 1}

This is the one of decimal numbers:
Σ = {0, ..., 9}

## Words

Words can be thought at as the concatenation of symbols within the alphabet.
For instance, this alphabet:
Σ = {A, ..., Z}

Has an infinite number of words that can be formed from, even though it has only 26 elements, with it we can form words such as:
- poop
- book
- fun

Every single word has something called its cardinality, which means, the number of elements in the alphabet that was used to form that word
For example, for the words we given before:
- |poop| = 4
- |book| = 4
- |fun| = 3

!Important!: Every single alphabet contains the λ word, that word is the empty element of alphabet algebra. Cardinality of λ is always 0 and λ is always inside the alphabet.

## A bit of ambiguity 

There is a bit of ambiguity in the concept of cardinality, for instance, let's assume the following alphabet:
Σ = {A, AA}

What would be the cardinality of the word AA? There are two solutions:
- If, we go for the naive solution, AA can be formed by combining A two times, therefore the cardinality is 2.
- But the more accepted solution, is that each element of the alphabet is unique and atomic, therefore, AA since it is a member of the alphabet has cardinality of 1.

## Operations on words

- Number of occurrences of `x` in `w`: Can be represented as |w|_x
  Important, is that x cannot be λ 
  This one is just the times the word x appeared in w, so let's assume the following alphabet:

  Σ = {A, ..., Z}

  And the word w = AA
  If x = B
  |w|_x = 0 since there is no B in the word AA 

  Now if w = AABABA
  |w|_x = 2 

  Another case of ambiguity is the following:
  Σ = {0, 1}
  w = 0111
  x = 11

  |w|_x = 2 because, even though there are 3 1's, we can make the symbol 11 2 times, one taking the first 1 and the second, and one taking the second and third. 

- Subword:
  Given the following alphabet:
  Σ = {0, 1}

  01 is a subword of 1011

  Subword can be divided in three parts, the subword can be either:
  - A prefix
  - An infix
  - A suffix

  In the case of 1011 01 is an infix, 10 is a prefix and 11 is a suffix
  λ is a subword of all words
  Also, given a word w, w is a subword of itself.

- I-th symbol of a word:
  We can use array syntax to do that, so w[i] = the element of the word at the index i. 1 <= i <= |w|

  Given the following alphabet:
  Σ = {0, 1}

  w = 011001
  `w[3]` = 1

- Concatenation:
  Is the binary operation between u and v that generates a new word w that is the juxtaposition of both.
  For instance:

  Σ = {0, 1}
  u = 0110
  v = 1001
  u.v = w = 01101001

  Concatenating every single word with λ gives itself, so:
  u . λ = u
  λ . u = u

  Also, |u.v| = |u|+|v|

  Concatenation is associative, therefore it forms a semigroup

  Concatenation is not commutative, therefore, the semigroup cannot be called abelian

- The power:
  The power of a word can be thought as repeated concatenation with itself, for instance

  Given:
  Σ = {0, 1}

  0^0 = λ 
  0^1 = 0
  0^2 = 00
  0^3 = 000
  ...

  So, w^n = w . w^n-1
  For n <= 1
  And w^0 = λ 

  Also, λ^n = λ


