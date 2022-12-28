[![CI - MSVC && GCC](https://github.com/KlasMvW/typesafe_coordinate_systems/actions/workflows/cmake.yml/badge.svg)](https://github.com/KlasMvW/typesafe_coordinate_systems/actions/workflows/cmake.yml)

# Type-safe coordinate systems

## Brief description

**TS** is header-only C++ library that allows the user to perform type-safe coordinate transformations within a hierarchy of systems. 

```
                A
              /  \
             B1  B2
           /  \   \
         C1   C2   C3
```
The only restriction on the hierarchy of systems is that each system can have only one parent system. Multiple child systems is however allowed. Each system (A, B1, B2 ...) is a user defined type and each transformation between two related systems is a user defined functions. Given this, the user can derive any transformation connecting two systems. In `TS` The transformation connecting `B2` and `C2` would be obtained by

```c++
M<float, ts::B2, ts::C2> m = ts::relate_systems<ts::B2, ts::C2, M>(g1, g2....);
```

where 

* `B2` and `C2` are user defined structs representing systems.
* `M` is a user defined type that is templated with the systems that is connected. Typically this is a class representing a matrix.
* `g1`, `g2` ... Are an arbitrary number of object instances of user defined types that can be used to transport information, like parameterization of the current geometry state, to the `to_parent`, `to_child` functions.
* `float` is the underlying data type of the `M`. Since `M` is user defined this parameter can be chosen freely..  

In the example above only the functions performing the transformations  `B2` to `A`, `A` to `B1` and `B1` to `C2` need to be defined. `TS` will traverse the tree and deduce the sequence of transformation `B2` to `C2` compile time.

# Simple example

`TS` uses an inheritance pattern to represent parent-child relation between two coordinate systems. The following construction in `TS` defines three systems, A, B and C, where A is the parent of B and C. 

```c++
namespace ts
{
  //    A
  //  /  \
  // B    C
  //
  struct A : TypesafeSystem<A, Root>{};
  struct B : TypesafeSystem<B, A>{};
  struct C : TypesafeSystem<C, A>{};
}
```

Here `TypesafeSystem` and `Root` are structs in the `TS` library in the namespace `ts`. The system in the top of the hierarchy (in this case system A) must have the parent `Root`. Note that the construct uses CRTP (Curiously Re-occurring Template Pattern) so that every system is inheriting from `TypesafeSystem` templated by the system itself.

Further, to use TS the user must define a matrix class templated by the system a matrix transforms *from* to the system it transforms *to* e.g.

```c++
template <typename T, typename From, typename To>
class Matrix {
  // Enter implementation of matrix calss here.
}

```
where `From` and `To` inherits from `ts::TypesafeSystem` like `A` and `B` in our example. `T` is the underlying data type (like `float`).
Note that the class does *not* need to be called `Matrix` but can have any name as long as the template signature is as described.

The functions defining the transformations from `A` to `B` and vice versa must have the following signatures

```c++
namespace Transform
{
template <typename T, typename From>
Matrix<T, From, A> to_parent(const Matrix<T, From, B>& in, const G1& g1, const G2& g2, ...);

template <typename T, typename From>
void to_child(const Matrix<T, From, A>& from, Matrix<T, From, B>& to, const G1& g1, const G2& g2, ...);
}
```
With the above definitions the matrix connecting two arbitrary systems in an hierarchy can be obtained by

```c++
Matrix<float, ts::A, ts::B> m = ts::relate_systems<ts::A, ts::B, Matrix>(g1, g2, ...);
```
*Note* that the arguments `g1, g2, ...` must match the signature of the `to_parent` and `to_child` functions. 

## Details

### The `to_parent` `to_child` functions

An example of `to_parent` and `to_child` functions are shown below.

```c++
template <typename T, typename From>
Matrix<T, From, A> to_parent(const Matrix<T, From, B>& from_to_B, const G1& g1, const G2& g2) {
  // Perform transformation concatenation of transforms here.

  // Construct the transformation Matrix that connects A and B.
  // The transformation is be parametrized by g1, g2
  // Here f is a function that returns the desired matrix.
  // The matrix could also be defined directly.
  const Matrix<T, B, A> B_to_A = f(g1, g2);
  
  // The Matrix class would typically have a multiplication operation defined that can guarantee type safety.
  Matrix<T, From, A> from_to_A = B_to_A * from_to_B;
  
  return from_to_A;
};

template <typename T, typename From>
void to_child(const Matrix<T, From, A>& from_to_A, Matrix<T, From, B>& from_to_B, const G1& g1, const G2& g2) {
  // Perform transformation concatenation of transforms here.

  // Construct the transformation Matrix that connects A and B.
  // The transformation is be parametrized by g1, g2
  // Here h is a function that returns the desired matrix. 
  // The matrix could also be defined directly.
  const Matrix<T, A, B> A_to_B = h(g1, g2);
  
  // The Matrix class would typically have a multiplication operation defined that can guarantee type safety.
  Matrix<T, From, B> from_to_B = A_to_B * from_to_A;
  
  return from_to_B
};
```


### Inclusion of *TS* headers

Source code using *TS* must include headers and function definitions in the following order
```c++
#include "ts/typesafe_system.h"

namespace ts::transform {
template <typename T, typename From>
Matrix<T, From, A> to_parent(const Matrix<T, From, B>& in, const G1& g1, const G2& g2) {};

template <typename T, typename From>
void to_child(const Matrix<T, From, A>& from, Matrix<T, From, B>& to, const G1& g1, const G2& g2) {};
}

#include "ts/typesafe_coordinate_systems.h"
```

This is normally achieved by putting the inclusion of `"ts/typesafe_system.h"` and the `to_parent` and `to_child` functions is a separate header that in turn is included before `#include "ts/typesafe_coordinate_systems.h"`where needed.

### Requirements on the Matrix class

If a matrix object is constructed through
```c++
Matrix<float, From, From> m;
```
i.e the connected systems are identical, the object must have the desired identity property.

### Compilers

Currently the tests of **TS** will not pass using MSVC with the C++17 compilation flag. It will however pass using the C++20 flag. **TS** is officially tested with GCC, MSVC for each push to the repo. It is unofficially tested with clang.  

## Examples

To see how *TS* is used explicitly, see the `test` directory.

## TS applied to general tree structures

Note that **TS** can be applied to any tree-like hierarchy where there is some kind of property that connects two nodes (systems) going in a specific direction. The `Matrix` can be any object as long as the signatures are correct and the `to_parent` and `to_child` functions have the correct form.  