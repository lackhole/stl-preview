# /*
#  * Created by YongGyu Lee on 2021/09/03.
#  */
#
# ifndef PREVIEW_TYPE_TRAITS_NEGATION_H_
# define PREVIEW_TYPE_TRAITS_NEGATION_H_
#
# include <type_traits>
# 
# include "preview/__core/inline_variable.h"
# include "preview/__type_traits/bool_constant.h"

namespace preview {

template<typename B> struct negation : bool_constant<!bool(B::value)> {};

template<typename B>
PREVIEW_INLINE_VARIABLE constexpr bool negation_v = negation<B>::value;

} // namespace preview

# endif // PREVIEW_TYPE_TRAITS_NEGATION_H_
