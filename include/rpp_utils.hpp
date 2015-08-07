#ifndef RPP_UTILS_HPP
#define RPP_UTILS_HPP

#include <functional>

namespace rpp {

namespace utils {

template<class...>
struct make_void { using type = void; };

template<class... Args>
using void_t = typename make_void<Args...>::type;

template <bool Head, bool... Tail>
using bool_constant = std::integral_constant<bool, Head && Tail...>;

template <int N>
using int_constant = std::integral_constant<int, N>;

template <class...>
struct typelist;

template <class, class>
struct typelist_cat;

template <std::size_t, class>
struct typelist_select;

template <std::size_t, class>
struct typelist_left;

template <>
struct typelist<> {};

template <class T>
struct typelist<T> {
    using Head = T;
};

template <class T, class... Rest>
struct typelist<T, Rest...> {
    using Head = T;
    using Tail = typelist<Rest...>;
};

template <class... Left, class... Right>
struct typelist_cat<typelist<Left...>, typelist<Right...>>
{
    using type = typelist<Left..., Right...>;
};

template <std::size_t N, class Head, class... Tail>
struct typelist_select<N, typelist<Head, Tail...>>
{
    using type = typelist_select<N-1, typelist<Tail...>>;
};

template <class Head, class... Tail>
struct typelist_select<0, typelist<Head, Tail...>>
{
    using type = Head;
};

template <std::size_t N, class Head, class... Tail>
struct typelist_left<N, typelist<Head, Tail...>>
{
    using type = typename typelist_cat< typelist<Head>
                                      , typename typelist_left<N-1, typelist<Tail...>>::type
                                      >::type;
};

template <class Head, class... Tail>
struct typelist_left<0, typelist<Head, Tail...>>
{
    using type = typelist<>;
};

template <>
struct typelist_left<0, typelist<>>
{
    using type = typelist<>;
};

template <class TypeListLeft, class TypeListRight>
using typelist_cat_t = typename typelist_cat<TypeListLeft, TypeListRight>::type;

template <std::size_t N, class TypeList>
using typelist_select_t = typename typelist_select<N, TypeList>::type;

template <std::size_t N, class TypeList>
using typelist_left_t = typename typelist_left<N, TypeList>::type;

template <std::size_t N, class... Args>
using select_nth_t = typelist_select_t<N, typelist<Args...>>;

template <std::size_t N, class... Args>
using left_n_t = typelist_left_t<N, typelist<Args...>>;

template <class Func, class... Args, std::size_t... Is>
inline auto invoke_tuple(Func &&f, std::tuple<Args...>&& args, std::index_sequence<Is...>)
    -> decltype(std::forward<Func>(f)(std::forward<Args>(std::get<Is>(args))...))
{
    return std::forward<Func>(f)(std::forward<Args>(std::get<Is>(args))...);
}

template <class Func, class... Args>
inline auto invoke_tuple(Func &&f, std::tuple<Args...>&& args)
    -> decltype(invoke_tuple(std::forward<Func>(f),
                             std::forward<std::tuple<Args...>>(args),
                             std::index_sequence_for<Args...>{}))
{
    return invoke_tuple(std::forward<Func>(f),
                        std::forward<std::tuple<Args...>>(args),
                        std::index_sequence_for<Args...>{});
}

} // namespace utils

}

#endif // RPP_UTILS_HPP























