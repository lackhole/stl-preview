# /*
#  * Created by YongGyu Lee on 2020/12/08.
#  */
#
# ifndef PREVIEW_TYPE_TRAITS_VOID_T_H_
# define PREVIEW_TYPE_TRAITS_VOID_T_H_
#
# include <type_traits>

namespace preview {

template<typename ...>
using void_t = void;

} // namespace preview

# endif // PREVIEW_TYPE_TRAITS_VOID_T_H_
