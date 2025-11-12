# Kleene Star

The Kleene star is one operator that is defined as follows:
Given a language Σ 

Σ^0 = {λ}
Σ^1 = Σ
Σ^i+1 = { w . v | w ∈ Σ^i and v ∈ Σ}, for each i > 0

So, the definition can be represented as follows:
Σ^* = Σ^0 ∪ Σ^1 ∪ Σ^2 ∪ Σ^3 ∪ ... 

For instance, let's consider the alphabet:
Σ = {0, 1}
Σ^* = {λ, 0, 1, 00, 11, 01, 10, 000, 001, 010, 011, 100, 101, 110, 111, ...}

# Definition of language

A language is just a set of words over one alphabet, for instance, we can think of a language such as:

Σ = {0, 1}
L = Σ^* 

So, in this case, L is a language that can take any permutation of the binary digits.

Also, let's say we want a language that only has binary words that have an even number of digits, to formalize it we would do this:

Σ = {0, 1}
L = { w | w ∈ Σ^* and |w| mod 2 = 0 }

This language has words such as:
- 0101
- 01
- 100001

But it does not have words such as:
- 1
- 1231231
- etc.

## Cool implication of the definition of language

A language is either:
- Equal to kleene star over the alphabet
- Or contained into it

# Concatenation of languages

The concatenation operation of two languages, generates a new language such that all the words can be formed
by concatenating words from one language onto the other. Forming thus a semigroup.

Language concatenation has the following properties:
- Identity: The empty language: {λ}, if we take any language L, L.{λ} = {λ}.L = L 
- Associativity: Since concatenation is associative, language concatenation is as well L_1.(L_2.L_3) = (L_1.L_2).L_3
- Non Abelian: Since concatenation is non-abelian over words, it is also the case with languages

We can represent repeated concatenation by using the power notation, just like we do with words.

# Kleene Star

The Kleene star is an operation on a language L and generates a new language L^* such that:
- L^0 = {λ}
- L^1 = L 
- L^(i+1) = {w.v | w ∈ L^i and v ∈ L}

The kleene star is the union of all L's powers. For instance, let's take a language L = {a}
L^* = {λ, a, aa, aaa, aaaa, aaaaa, ...}

We can also define the Kleene star without the empty word, it is usually denoted by L^+

