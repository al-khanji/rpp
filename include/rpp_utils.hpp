#ifndef RPP_UTILS_HPP
#define RPP_UTILS_HPP

#pragma once

#include <functional>

namespace rpp {

namespace utils {

template <class...>
using void_t = void;

template <bool Head, bool... Tail>
using bool_constant = std::integral_constant<bool, Head && Tail...>;

template <int N>
using int_constant = std::integral_constant<int, N>;

template <class... Args>
struct List {};

template <typename Head, typename... Tail>
struct List<Head, Tail...>
{
    typedef Head Car;
    typedef List<Tail...> Cdr;
};

template <class, class>
struct List_Cat;

template <class... Left, class... Right>
struct List_Cat<List<Left...>, List<Right...>>
{
    typedef List<Left..., Right...> type;
};

template <std::size_t N, class... Args>
struct List_Left
{
    using type = typename List_Left<N, List<Args...>>::type;
};

template <std::size_t N, class Head, class... Tail>
struct List_Left<N, List<Head, Tail...>>
{
    using type = typename List_Cat< List<Head>
                                  , typename List_Left<N-1, Tail...>::type
                                  > :: type;
};

template <class... Ts>
struct List_Left<0, Ts...>
{
    typedef List<> type;
};

template <std::size_t N, class Head, class... Tail>
struct List_Select
{
    typedef typename List_Select<N-1, Tail...>::type type;
};

template <class Head, class... Tail>
struct List_Select<0, Head, Tail...>
{
    typedef Head type;
};

template <std::size_t N, class... Elements>
struct List_Select<N, List<Elements...>>
{
    typedef typename List_Select<N, Elements...>::type type;
};

namespace detail {

    struct do_can_call_impl
    {
        template <class F, class... Args,
                  class = decltype(std::declval<F>()(std::declval<Args>()...))>
        static std::true_type __test(int);

        template <class, class...>
        static std::false_type __test(...);
    };

    template <class F, class... Args>
    struct can_call_impl : do_can_call_impl
    {
        using type = decltype(__test<F, Args...>(0));
    };

} // namespace detail

template <class F, class... Args>
struct can_call : detail::can_call_impl<F, Args...>::type
{};

namespace detail {
    template <class F, class... Args>
    struct accepts_n_of_impl;

    template <class F, class... Args>
    struct accepts_n_of_impl<F, List<Args...>>
    {
        enum {
            n_args = sizeof...(Args)
        };

        using type = std::conditional_t < can_call<F, Args...>::value
                                        , int_constant<n_args>
                                        , typename accepts_n_of_impl < F
                                                                     , typename utils::List_Left<n_args - 1
                                                                                                 , Args...> :: type
                                                                     > :: type
                                        >;
    };

    template <class F>
    struct accepts_n_of_impl<F, List<>>
    {
        using type = std::conditional_t < can_call<F>::value
                                        , int_constant< 0>
                                        , int_constant<-1>
                                        >;
    };

} // namespace detail

template <class F, class... Args>
struct accepts_n_of : detail::accepts_n_of_impl<F, List<Args...>>::type
{};

template <std::size_t N, class Head, class... Elements>
struct nth_element
{
    typedef typename nth_element<N-1, Elements...>::type type;
};

template <class Head, class... Tail>
struct nth_element<0, Head, Tail...>
{
    typedef Head type;
};

template <std::size_t N, class... Elements>
using nth_element_t = typename nth_element<N, Elements...>::type;

template <class Callable, class... EmissionArgs, std::size_t... Is>
auto apply_emission(Callable fn, void **args, List<EmissionArgs...>, std::index_sequence<Is...>)
{
    (void) args[0]; // we always have at least one element in the array, shut up the compiler warnings about unused
                    // variables for functions that take no arguments

    return std::forward<Callable>(fn)(*reinterpret_cast<nth_element_t<Is, EmissionArgs...> *>(args[Is])...);
}

} // namespace utils

}

#endif // RPP_UTILS_HPP

