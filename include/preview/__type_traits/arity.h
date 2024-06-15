# /*
#  * Created by YongGyu Lee on 2020/12/08.
#  */
#
# ifndef PREVIEW_TYPE_TRAITS_ARITY_H_
# define PREVIEW_TYPE_TRAITS_ARITY_H_
#
# include <cstddef>
# include <type_traits>
# include <utility>
# include "preview/__core/inline_variable.h"
# include "preview/__type_traits/void_t.h"
# include "preview/__type_traits/is_invocable.h"
# include "preview/__utility/type_sequence.h"

namespace preview {

template <typename T>
struct arity;

namespace detail {

template<typename T, typename = void>
struct has_non_template_operator_call : std::false_type {};
template<typename T>
struct has_non_template_operator_call<T, void_t<decltype(&T::operator())>> : std::true_type {};

struct ubiq {
  template<typename T>
  operator T() const noexcept;
};

template<typename F, typename Seq>
struct is_template_invocable_n;

template<typename F, typename... Ubiq>
struct is_template_invocable_n<F, type_sequence<Ubiq...>>
    : is_invocable<F, Ubiq...> {};

template<
  typename T,
  std::size_t Max,
  std::size_t Tested = 0,
  bool Invocable = is_template_invocable_n<T, make_type_sequence<ubiq, Tested>>::value /* false */>
struct has_template_operator_call
    : std::conditional_t<
        (Tested < Max),
        has_template_operator_call<T, Max, Tested + 1>,
        std::integral_constant<std::size_t, 0>
    > {};

template<typename T, std::size_t Max, std::size_t Tested>
struct has_template_operator_call<T, Max, Tested, true>
    : std::integral_constant<std::size_t, Tested> {};

struct no_arity {};

template<typename T, bool = std::is_class<T>::value /* false */, bool = has_non_template_operator_call<T>::value>
struct arity_class;

template<typename T>
struct arity_class<T, true, true>
    : arity<decltype(&T::operator())> {};

template<typename T>
struct arity_class<T, true, false>
    : std::conditional_t<
        has_template_operator_call<T, 20>::value != 0,
        has_template_operator_call<T, 20>,
        no_arity
    > {};

} // namespace detail

template <typename T>
struct arity : detail::arity_class<T>{};

template<typename R, typename... Args>
struct arity<R(Args...)> : std::integral_constant<std::size_t, sizeof...(Args)> {};

template <typename R, typename... Args>
struct arity<R(*)(Args...)> : std::integral_constant<std::size_t, sizeof...(Args)> {};

template <typename R, typename C, typename... Args>
struct arity<R(C::*)(Args...)> :
    std::integral_constant<std::size_t, sizeof...(Args)> {};

template <typename R, typename C, typename... Args>
struct arity<R(C::*)(Args...) const> :
    std::integral_constant<std::size_t, sizeof...(Args)> {};

template<typename T>
PREVIEW_INLINE_VARIABLE constexpr std::size_t arity_v = arity<T>::value;

} // namespace preview

#endif // PREVIEW_TYPE_TRAITS_ARITY_H_
