# /*
#  * Created by YongGyu Lee on 2024/01/10.
#  */
#
# ifndef PREVIEW_TYPE_TRAITS_IS_EXPLICITLY_CONVERTIBLE_H_
# define PREVIEW_TYPE_TRAITS_IS_EXPLICITLY_CONVERTIBLE_H_
#
# include <type_traits>
#
# include "preview/__type_traits/void_t.h"

namespace preview {

template<typename From, typename To, typename = void>
struct is_explicitly_convertible : std::false_type {};

template<typename From, typename To>
struct is_explicitly_convertible<From, To, void_t<decltype(static_cast<To>(std::declval<From>()))>> : std::true_type {};

} // namespace preview

# endif // PREVIEW_TYPE_TRAITS_IS_EXPLICITLY_CONVERTIBLE_H_
