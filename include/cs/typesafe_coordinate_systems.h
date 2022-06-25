#pragma once

#include <type_traits>

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
}

namespace FindFunction
{
    template<typename Arg1, typename Arg2>
	constexpr auto toChild(int) -> decltype(Transform::toChild(std::declval<Arg1>(), std::declval<Arg2>()), std::true_type{})
    {
        return {};
    }
	
    template<typename Arg1, typename Arg2>
    constexpr auto toChild(...) -> std::false_type
    {
        return {};
    }
}

template<typename S1, typename S2>
constexpr auto findCommonAncestorSystem()
{
    if constexpr (S1::level > S2::level)
    {
        return findCommonAncestorSystem<typename S1::parent, S2>();
    }   
    else if constexpr (S1::level < S2::level)
    {
        return findCommonAncestorSystem<S1, typename S2::parent>();
    }
    else
    {    
        if constexpr(std::is_same<S1, S2>::value)
        {
			return typename S1::system{};
        }
        else
        {
            return findCommonAncestorSystem<typename S1::parent, typename S2::parent>(); 
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
		const auto v1 = Transform::toParent(rhs);
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
	else if constexpr (FindFunction::toChild<const M<From, Intermediate> &, M<From, To> &>(0)){
		M<From, To> v1;
		Transform::toChild(rhs, v1);
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
M<From, To> relateSystems()
{
	M<From, From> sr;
	auto v1 = up<decltype(findCommonAncestorSystem<To, From>())>(sr);
	auto v2 = down<To>(v1);
	return v2;
}
