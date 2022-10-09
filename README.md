[![CI - MSVC && GCC](https://github.com/KlasMvW/typesafe_coordinate_systems/actions/workflows/cmake.yml/badge.svg)](https://github.com/KlasMvW/typesafe_coordinate_systems/actions/workflows/cmake.yml)

# Type-safe coordinate systems

## Brief description

**TS** is header-only C++ library that allows the user to perform coordinate transformations in a type-safe manner.
Each coordinate system is represented by a struct and the parent-child relation between the systems is represented by inheritance. The only restriction on the hierarchy of systems is that each system can have only one parent system.
Multiple child systems is however allowed.

```
                A
              /  \
             B1  B2
           /  \   \
         C1   C2   C3
```

The typesafe transformation matrix for a transformation between two systems is obtained through

```c++
Matrix<float, ts::A, ts::C2> m = ts::relateSystems<ts::A, ts::C2, Matrix>(g1, g2....);
```
where 


* A and C2 are system structs.
* Matrix is a user defined class that is templated with the systems that the matrix connects.
* g1, g2... Are an arbitrary number of objects of user defined types that holds the current geometry state.

Besides the system structs, the matrix class and the geometry objects, the user must define free functions with specific signatures that performs the transformation between two connected systems in the hierarchy.

# Simple example

Two coordinate systems, A and B, with a parent-child relation, i.e. there exists a transformation that takes an entity defined in A to a representation in B and vice versa, are represented by the following construction in `TS`. 

```c++
namespace ts
{
  struct A : System<A, Root>
  {
  };

  struct B : System<B, A>
  {
  };
}
```

Here `System` and `Root` are structs in the TS library in the namespace `ts`. The system in the top of the hierarchy (in this case system A) must inherit from `Root` . Note that the construct uses CRTP (Curiously Re-occuring Template Pattern) so that every system is inheriting from `System` templated by the system itself.

Further, to use TS the user must define a matrix class templated by the system a matrix transforms from to the system it transforms to e.g.

```c++
template <typename T, typename From, typename To>
class Matrix {
  ...
}

```
where `From` and `To` inherits from `ts::System` like `A` and `B` in our example. `T` is the underlying data type (like `float`).
Note tha the class does *not* need to be called `Matrix` but can have any name as long as the template signature is as described.

The functions defining the transformations from `A` to `B`  and vice versa shall have the following signatures

```c++
namespace Transform
{
template <typename T, typename From>
Matrix<T, From, A> toParent(const Matrix<T, From, B>& in, const G1& g1, const G2& g2, ...);

template <typename T, typename From>
void toChild(const Matrix<T, From, A>& from, Matrix<T, From, B>& to, const G1& g1, const G2& g2, ...);
}
```
With the above definitions the matrix connecting two arbitrary systems in an hierarchy can be obtained by

```c++
Matrix<float, ts::A, ts::B> m = ts::relateSystems<ts::A, ts::B, Matrix>(g1, g2, ...);
```
*Note* that the arguments `g1, g2, ...` must match the signature of the `toParent` and `toChild` functions. 

## Details

### Inclusion of TS headers

Source code using TS must include headers and function definitions in the following order
```c++
#include "ts/typesafe_system.h"

namespace ts::transform {
template <typename T, typename From>
Matrix<T, From, A> toParent(const Matrix<T, From, B>& in, const G1& g1, const G2& g2, ...);

template <typename T, typename From>
void toChild(const Matrix<T, From, A>& from, Matrix<T, From, B>& to, const G1& g1, const G2& g2, ...);
}

#include "ts/typesafe_coordinate_systems.h"
```

This is normally achieved by putting the inclusion of `"ts/typesafe_system.h"` and the `toParent` and `toChild` functions is a separate header that in turn is included before `#include "ts/typesafe_coordinate_systems.h"`where needed.

### Requirements on the Matrix class

If a matrix object is constructed through
```c++
Matrix<float, From, From> m;
```
i.e the connected systems are identical, the object must have the desired identity property.

### Compilers

Currently the tests of **TS** will not pass using MSVC with the c++17 compilation flag. It will however pass using the c++20 flag. **TS** is officially tested with GCC, MSVC for each push to the repo. It is unofficially tested with clang.  

## Examples

To see how *TS* is used explicitly, see the files in the `test` directory.

## TS applied to general tree structures

Note that **TS** can be applied to any tree-like hierarchy where there is some kind of property that connects two nodes (systems) going in a specific direction. The `Matrix` can be any object as long as the signatures are correct and the `to_parent` and `to_child` functions have the correct form.  