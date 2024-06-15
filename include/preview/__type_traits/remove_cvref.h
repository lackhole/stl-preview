# /*
#  * Created by YongGyu Lee on 2020/12/08.
#  */
#
# ifndef PREVIEW_TYPE_TRAITS_CVREF_H_
# define PREVIEW_TYPE_TRAITS_CVREF_H_
#
# include <type_traits>

namespace preview {

template<typename T>
struct remove_cvref {
  using type = std::remove_cv_t<std::remove_reference_t<T>>;
};

template<typename T>
using remove_cvref_t = typename remove_cvref<T>::type;

} // namespace preview

# endif // PREVIEW_TYPE_TRAITS_CVREF_H_
