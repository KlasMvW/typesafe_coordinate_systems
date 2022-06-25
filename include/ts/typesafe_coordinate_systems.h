#pragma once

#include <type_traits>

namespace Ts
{

namespace System
{
	template<typename ThisSystem, typename ParentSystem>
	struct System
	{
		static constexpr int level = ParentSystem::level + 1;
		using parent = ParentSystem;
		using system = ThisSystem;
	};

	struct Root
	{
		static constexpr int level = 0;
	};
} // namespace System

namespace Find_function
{
    template<typename Arg1, typename Arg2>
	constexpr auto to_child(int) -> decltype(Transform::to_child(std::declval<Arg1>(), std::declval<Arg2>()), std::true_type{})
    {
        return {};
    }
	
    template<typename Arg1, typename Arg2>
    constexpr auto to_child(...) -> std::false_type
    {
        return {};
    }
} // namespace Find_function

template<typename S1, typename S2>
constexpr auto find_common_ancestor()
{
    if constexpr (S1::level > S2::level)
    {
        return find_common_ancestor<typename S1::parent, S2>();
    }   
    else if constexpr (S1::level < S2::level)
    {
        return find_common_ancestor<S1, typename S2::parent>();
    }
    else
    {    
        if constexpr(std::is_same<S1, S2>::value)
        {
			return typename S1::system{};
        }
        else
        {
            return find_common_ancestor<typename S1::parent, typename S2::parent>(); 
        }
    }   
}

template<typename To, typename From, typename Intermediate, template<typename, typename> typename M>
M<From, To> up(const M<From, Intermediate> &rhs)
{
	if constexpr (std::is_same<To, Intermediate>::value)
	{
		return rhs;
	}
	else
	{
		const auto v1 = Transform::to_parent(rhs);
		const auto v2 = up<To>(v1);
		return v2;
	}
}

template<typename To, typename From, typename Intermediate, template<typename, typename> typename M>
M<From, To> down(const M<From, Intermediate> &rhs)
{
	if constexpr (std::is_same<To, Intermediate>::value)
	{
		return rhs;
	}
	else if constexpr (Find_function::to_child<const M<From, Intermediate> &, M<From, To> &>(0)){
		M<From, To> v1;
		Transform::to_child(rhs, v1);
		return v1;
	}
	else
	{
		auto v2 = down<typename To::parent, From>(rhs);
		auto v3 = down<To>(v2);
		return v3;
	}
}

template<typename From, typename To,  template<typename, typename> typename M>
M<From, To> relate_system()
{
	M<From, From> sr;
	auto v1 = up<decltype(find_common_ancestor<To, From>())>(sr);
	auto v2 = down<To>(v1);
	return v2;
}
} // namespace Ts
