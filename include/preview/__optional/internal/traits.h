# /*
#  * Created by YongGyu Lee on 2021/05/24.
#  */
#
# ifndef PREVIEW_OPTIONAL_INTERNAL_TRAITS_H_
# define PREVIEW_OPTIONAL_INTERNAL_TRAITS_H_
#
# include <type_traits>

namespace preview {
namespace internal {
namespace optional {

template<bool v>
struct conditional_tf : std::conditional_t<v, std::true_type, std::false_type> {};

template<bool v>
using conditional_tf_t = typename conditional_tf<v>::type;

template<typename Original, typename Other>
struct check_constructible :
  conditional_tf_t<
    !std::is_constructible<Original, Other & >::value && !std::is_constructible<Original, Other const& >::value &&
    !std::is_constructible<Original, Other &&>::value && !std::is_constructible<Original, Other const&&>::value> {};

template<typename Original, typename Other>
struct check_convertible :
  conditional_tf_t<
    !std::is_convertible<Other & , Original>::value && !std::is_convertible<Other const& , Original>::value &&
    !std::is_convertible<Other &&, Original>::value && !std::is_convertible<Other const&&, Original>::value> {};

template<typename Original, typename Other>
struct check_assignable :
  conditional_tf_t<
    !std::is_assignable<Original&, Other & >::value && !std::is_assignable<Original&, Other const& >::value &&
    !std::is_assignable<Original&, Other &&>::value && !std::is_assignable<Original&, Other const&&>::value> {};

template<typename T>
struct strip {
#if __cplusplus <= 201703
  using type = std::decay_t<T>;
#else
  using type = std::remove_cvref_t<T>;
# endif
};

template<typename T>
using strip_t = typename strip<T>::type;

} // namespace optional
} // namespace internal
} // namespace preview

# endif // PREVIEW_OPTIONAL_INTERNAL_TRAITS_H_
