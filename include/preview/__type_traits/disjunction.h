# /*
#  * Created by YongGyu Lee on 2021/09/03.
#  */
#
# ifndef PREVIEW_TYPE_TRAITS_DISJUNCTION_H_
# define PREVIEW_TYPE_TRAITS_DISJUNCTION_H_
#
# include <type_traits>

namespace preview {

template<typename ...B>
struct disjunction;

template<>
struct disjunction<> : std::false_type {};

template<typename B1>
struct disjunction<B1> : B1 {};

template<typename B1, typename ...BN>
struct disjunction<B1, BN...> : std::conditional_t<bool(B1::value), B1, disjunction<BN...>> {};

template<typename... B>
PREVIEW_INLINE_VARIABLE constexpr bool disjunction_v = disjunction<B...>::value;

} // namespace preview

# endif // PREVIEW_TYPE_TRAITS_DISJUNCTION_H_
