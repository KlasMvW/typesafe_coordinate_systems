#pragma once

#include <type_traits>

// This file implements type-safe coordinate transformations.
//
// Use it as follows:
//
// 1. Define types for all systems.
//    Every system is a unique type that derives from the class TypesafeSystem defined in typesafe_system.h.
//    A new system is declared through
//
//    struct NewSystem : TypesafeSystem<NewSystem, ParentSystem> {}
//
//    * A system must have one parent system only.
//    * A system can have many child systems.
//    * The root system must have the special pre-defined parent system Root. Root is defined in typesafe_system.h.
//
// 2. Define a matrix class with the following signature
//
//    template<typename T, typename From, typename To>
//    class M;
//
//    Here T is the underlying data type of the matrix class, From and To are systems defined under point 1.
//
//    * The name of the class does not need to be M but can take any name you wish.
//    * The default constructor of M must have the unit property if From and To are the same type.
//
// 3. Define free functions that return the transformation matrix connecting all child-parent pairs here called A and B where A is the parent system
// of B.
//    The functions must exists in namespace 'transform'. The signature and functionality of the functions is the following
//
//    template<typename T, typename From>
//    M<T, From, A> to_parent(const M<T, From, B>& from,
//                           GeometryObjects...) {
//     M<T, B, A> B_to_A = /*Explicitly assign B_to_A probably dependent on the geometry objects*/;
//     M<T, From, A> sr = /*Apply binary operation between B_to_A and from that creates M<T, From, A>. Normally this is matrix multiplication*/;
//     return sr;
//    }
//
//    template<typename T, typename From>
//    void to_child(const TSMatrix<T, From, A>& from,
//                 TSMatrix<T, From, B>& to,
//                 GeometryObjects...)
//    {
//      TSMatrix<T, system::A, system::B> A_to_B; /*Explicitly assign B_to_A probably dependent on the geometry objects*/;;
//      to = /*Apply binary operation between B_to_A and from that creates M<T, From, A>. Normally this is matrix multiplication*/;
//    }
//
//    Here T is the underlying data type of the matrix class. From is a system defined under 1.
//
//    * The functions can take an arbitrary number of geometry objects. All functions must however have the same geometry input types.
//
// 4. Use the public interface function 'relate_systems' to obtain a matrix object relating any two systems defined under 1.
//    A call could have the form
//
//    M<float, A, Z> m = relate_systems<float, A, Z, M>(geometryObject1, geometryObject2);
//
//    * The geometry objects must have the exact types that are used in the signatures of the to_parent to_child functions.
//
// Explanation of the algorithm
//
// The algorithm deduces the sequence of function calls required to obatin ant transformation matrix between two systems at compile time.
// The computation of the matrix is done run time since the actual coputation depends on runtime data.
//
// Given a hierarchy like
/*
             A1
           / |  \
         B2  C2  D2
            /  \   \
           E3  F3   G3
*/
// the call
// M<float, G3, F3> m = relateSystem<float, G3, F3, M>(someGeometryStruct);
// works as follows:

// 1. Find the common ancestor system of G3 and F3.
//    This is done at compile time. This is possible since the level of a system (1, 2, 3...) in the hierarchy is known
//    implicitly when constructed through the base class 'System'. Since the parent system of a system also is known
//    implicitly at construction (again through the base class) we can recursively walk through the hierarchy from the two
//    initial systems utill we find a system that is a grand-(grand...)parent of both systems. This is done with the 'find_common_ancestor()'
//    function.
//
// 2. Construct a matrix with unit property of the type M<float, G3, G3> (assuming underlying dataype is float).
//    Since the parent of G3 (D2) is known to G3 by construction, the corresponding to_parent function can be called yielding
//    M<float, G3, D2>. This is repeated until we reach the common ancestor, A1, of G3 and F3 i.e compile time we reach the sequence of
//    function calls that yields M<float, G3, A1>. This is a recursive procedure. The function responsible for this is the template function 'up'.
//
// 3. Obtain the transformation from A1 to F3 by finding all the involved 'to_child' functions.
//    This is more complicated than finding all 'to_parent' functions as under 1. This is because
//    the parent systems are not aware of their children.
//    This a recursive procedure and the responsible function is 'down'.
//    To detect existing to_child functions we use SFINAE (Substitution Failure Is Not An Error).
//    The responsible functions are the overoaded find_function::to_child functions.

namespace ts{
namespace find_function {

// Compile time check if a to_child function with specific arguments exists.
namespace internal {

// Define template variable to_child that defaults to false.
template <typename, typename, typename = void, typename...>
constexpr bool to_child = false;

// Specialization of templateVariable to_child.
// The template argument std::void_t<decltype(transform::to_child(std::declval<Arg1>(), std::declval<Arg2>(), std::declval<GeometryState>()...))>
// has the 'type' ´void´ if the decltype expression is valid after substitution.
template <typename Arg1, typename Arg2, typename... GeometryState>
constexpr bool to_child<Arg1,
                       Arg2,
                       std::void_t<decltype(transform::to_child(std::declval<Arg1>(), std::declval<Arg2>(), std::declval<GeometryState>()...))>,
                       GeometryState...> = true;
} // namespace internal

// Interface variable to internal::to_child.
// template arguments, Arg1, Arg2, void, GeometryState..., will match the specialization above if the decltype expression can be evaluated and
// find_function::to_child will evaluate to true. Else find_function::to_child will be false.
template <typename Arg1, typename Arg2, typename... GeometryState>
constexpr bool to_child = internal::to_child<Arg1, Arg2, void, GeometryState...>;
} // namespace find_function

// Compile time detection of the common ancestor type of S1 and S2.
// Note that trying to two find the commin ancestor of disconnected trees will result in compilation failure although the trees share the same
// root node of type Root (se the difinition of class TypesafeSystem and class Root).
// This is because Root is not a TypesafeSystem and the function below requires the returned type to be a TypesafeSystem.
template <typename S1, typename S2>
constexpr auto find_common_ancestor() {
  if constexpr (S1::level > S2::level) {
    return find_common_ancestor<typename S1::Parent, S2>();
  } else if constexpr (S1::level < S2::level) {
    return find_common_ancestor<S1, typename S2::Parent>();
  } else {
    if constexpr (std::is_same<S1, S2>::value) {
      // The returned type is a TypesafeSystem.
      // The line below will render a compilation error if we try to find the common ancestor of nodes in different trees.
      return typename S1::System{};
    } else {
      return find_common_ancestor<typename S1::Parent, typename S2::Parent>();
    }
  }
}

// Compile time recursive function that returns the compund transformation matrix from system From to system To
// when To is a parent or grand parent of From. This is true if there exists a set of to_parent functions that
// connects From and To.
template <typename T,
          typename To,
          typename From,
          typename Intermediate,
          template <typename, typename, typename>
          typename M,
          typename... GeometryState>
M<T, From, To> up(const M<T, From, Intermediate>& rhs, [[maybe_unused]]const GeometryState&... geometryState) {
  if constexpr (std::is_same<To, Intermediate>::value) {
    return rhs;
  } else {
    const auto v1 = transform::to_parent(rhs, geometryState...);
    const auto v2 = up<T, To>(v1, geometryState...);
    return v2;
  }
}

// Compile time recursive function that returns the compund transformation matrix from system From to system To
// when To is a child or grand child of From.
template <typename T,
          typename To,
          typename From,
          typename Intermediate,
          template <typename, typename, typename>
          typename M,
          typename... GeometryState>
M<T, From, To> down(const M<T, From, Intermediate>& rhs, [[maybe_unused]]const GeometryState&... geometryState) {
  if constexpr (std::is_same<To, Intermediate>::value) {
    return rhs;
  } else if constexpr (find_function::to_child<const M<T, From, Intermediate>&, M<T, From, To>&, const GeometryState&...>) {
    M<T, From, To> v1;
    transform::to_child(rhs, v1, geometryState...);
    return v1;
  } else {
    auto v2 = down<T, typename To::Parent, From>(rhs, geometryState...);
    auto v3 = down<T, To>(v2, geometryState...);
    return v3;
  }
}

// Public interface to type-safe transformations
// The variadic argument should be the same as is used by the to_child and to_parent functions defined in the transform namespace.
template <typename T, typename From, typename To, template <typename, typename, typename> typename M, typename... GeometryState>
M<T, From, To> relate_systems(const GeometryState&... geometryState) {
  // sr must be unity when constructed.
  M<T, From, From> sr;

  // Get the transformation from From to the common ancestor of From and To
  auto v1 = up<T, decltype(find_common_ancestor<From, To>())>(sr, geometryState...);

  // Get the transformation connecting From and To by applying the transformation from the
  // common ancestor of From and To to To.
  auto v2 = down<T, To>(v1, geometryState...);
  return v2;
}
} // namespace ts