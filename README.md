# Typesafe coordinate systems

## Brief description

**TS** is header-only C++ library that allows the user to perform coordinate transformations in a typesafe manner.
Each coordinate system is represented by a struct and the parent-child relation between the systems is represented by inheritance. The only restriction on the hierarchy of systems is that each system can have only one parent system.
Multiple child systems is however allowed.

```
                A
              /  \
             B1  B2
           /  \   \
         C1   C2   C3
```

Defining the systems, a matrix class templated with the system the matrix connects and free functions that defines the transformations between a parent and a child system, the transformation matrix for a transformation between two systems is obtained through

```c++
Matrix<Ts::System::A, Ts::System::B> m = Ts::relate_systems<Ts::System::A, Ts::System::B, Matrix>();
```

# Simple example

Two coordinate systems, A and B, with a parent-child relation, i.e. there exists a transformation that takes an entity defined in A to a representation in B and vice versa, are represented by the following construction in `TS`. 

```c++
namespace Ts::System
{
  struct A : System<A, Root>
  {
  };

  struct B : System<B, A>
  {
  };
}
```

Here `System` and `Root` are structs in the TS library in the namespace `Ts::System`. The system in the top of the hierarchy must inherit from `Root` (in this case system A). Note that the construct uses CRTP (Curiously Reoccuring Template Pattern) so that every system is inheriting from `System` templated by the system itselfe.

Further, to use TS the user must define a matrix class templated by the system a matrix transforms from to the system it transforms to e.g.

```c++
template <typename From, typename To>
class Matrix {
  ...
}

```
where `From` and `To` inherits from `Ts::System::System` like `A` and `B` in our example.
Note tha the class does *not* need to be called `Matrix` but can have any name as long as the template signature is as described.

The functions defining the transformations from `A` to `B`  and vice versa shall have the following signatures

```c++
namespace Transform
{
  template<typename From>
  Matrix<From, System::A> to_parent(const Matrix<From, System::B>& from);

  template<typename From>
  void to_child(const Matrix<From, System::A>& from, Matrix<From, System::B>& to);
}
```
With the above definitions the matrix connection tow arbitrary systems in an hierarchy can be obtained by

```c++
Matrix<Ts::System::A, Ts::System::B> m = Ts::relate_systems<Ts::System::A, Ts::System::B, Matrix>();
```