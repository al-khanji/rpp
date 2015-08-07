#ifndef RPP_CONCEPTS_HPP
#define RPP_CONCEPTS_HPP

#include "rpp_utils.hpp"

namespace rpp {
namespace concepts {

struct Callable {
    template <class F, class... Args>
    auto requires_(F &&f, Args&&... args)
    -> decltype( f(std::forward<Args>(args)...) );
};

template <class Concept, class Enable = void>
struct models : std::false_type {};

template <class Concept, class... Ts>
struct models
    < Concept(Ts...)
    , utils::void_t<decltype(std::declval<Concept>().requires_(std::declval<Ts>()...))>
    > : std::true_type {};

} // namespace concepts
} // namespace rpp

#define MODELS_CONCEPT(...) concepts::models<__VA_ARGS__>::value
#define REQUIRES_CONCEPT(...) typename std::enable_if<rpp::concepts::models<__VA_ARGS__>::value, int>::type = 0

#endif // RPP_CONCEPTS_HPP

