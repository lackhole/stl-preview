#ifndef PREVIEW_TYPE_TRAITS_CONDITIONAL
#define PREVIEW_TYPE_TRAITS_CONDITIONAL

#include <type_traits>

namespace preview {

/// @brief extended conditional traits that supports if-elif-...-else chain
template<typename B, typename IfRes, typename ElseIfRes, typename...>
struct conditional;

template<typename B, typename IfRes, typename ElseRes>
struct conditional<B, IfRes, ElseRes> {
    using type = std::conditional_t<bool(B::value), IfRes, ElseRes>;
};

template<typename B1, typename IfRes, typename B2, typename ElseIfRes, typename... Rest>
struct conditional<B1, IfRes, B2, ElseIfRes, Rest...> {
    using type = std::conditional_t<
        bool(B1::value), IfRes,
        typename conditional<B2, ElseIfRes, Rest...>::type
    >;
};

template<typename B, typename IfRes, typename B2, typename... ElseIfRes>
using conditional_t = typename conditional<B, IfRes, B2, ElseIfRes...>::type;

} // namespace preview

#endif // PREVIEW_TYPE_TRAITS_CONDITIONAL
