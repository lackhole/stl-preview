# /*
#  * Created by YongGyu Lee on 2021/09/03.
#  */
#
# ifndef PREVIEW_TYPE_TRAITS_BOOL_CONSTANT_H_
# define PREVIEW_TYPE_TRAITS_BOOL_CONSTANT_H_
# 
# include <type_traits>

namespace preview {

template<bool v>
using bool_constant = std::integral_constant<bool, v>;

template<typename ...> struct always_false : std::false_type {};
template<typename ...> struct always_true : std::true_type {};

} // namespace preview

# endif // PREVIEW_TYPE_TRAITS_BOOL_CONSTANT_H_
