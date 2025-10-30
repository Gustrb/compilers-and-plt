# Subgroups

Subgroups are just like **Magmas** but they have one extra requirement. For a magma to be considered a *Semigroup*
its binary operation must be associative.

## Associativity

Associativity is a really important requirement of a Semigroup, it states that:

For any a, b, c in the Set. (a . (b . c)) = ((a . b) . c)
So, it means that no matter the order I perform the applications, the result is the same.

Important: Associativity is not the same of commutativity. A semigroup is *Not* commutative, so we cannot say the following:

For any, a and b, a . b = b . a

This proposition does not hold inside semigroups.
If it does, it means the semigroup is called an **Abelian** semigroup, which means it has some extra structure.

### Examples

1. Numbers with +
  Numbers form a semigroup with addition, since we know that, for any a, b in the Natural numbers,
  a + b is also a natural number, thus addition is a binary operation in the natural numbers.
  It is trivially true, as well, that addition of numbers is associative.

  It is also true that numbers with + are also an Abelian semigroup, since for any a, b in the Natural numbers
  a + b = b + b.
  Note: in computers, it is not true that floats (and doubles) are, either associative nor commutative, but that is an implementation detail due to IEEE-754. 

2. Numbers with *
  In the same way that the natural numbers form a semigroup with addition, it is also the case they form a semigroup with multiplication. Not only that, but it is also an Abelian Semigroup since multiplication is commutative;

3. Booleans with & and |
  Boolean values (either 0 or 1) also form a subgroup with both & and |.

  We can prove that & is associative by contradiction, assume that, there is a, b, c in the boolean numbers such that:
  a . (b . c) != (a . b) .c
  For this to hold, one side must be 0 and the other must be 1.

  Let's assume that a . (b . c) = 1
  Therefore, a, b, c must be = 1

  And for the left-hand side to be different to the right-hand side, (a . b) . c has to equal 0
  And for that to be the case, either a, b or c would have to be 0, which contradicts the left-hand side.
  Therefore, it is proven that booleans are associative with the &. It is trivially true that | is also associative.

4. Strings with string concatenation
  It is super interesting, but strings also form a semigroup with the binary operation of string concatenation, more interesting than that is the fact that string concatenation is not commutative, under the set of all strings. Therefore we can't say this is an Abelian Semigroup.

## References:
- Pragmatic Category Theory | Part 1: Semigroup Intro [https://chshersh.com/blog/2024-07-30-pragmatic-category-theory-part-01.html]
