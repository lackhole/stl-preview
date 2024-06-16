# /**
#  * Created by YongGyu Lee on 2024. 6. 16..
#  */
#
# ifndef PREVIEW_ITERATOR_DETAIL_STD_CHECK_H_
# define PREVIEW_ITERATOR_DETAIL_STD_CHECK_H_
#
# include <iterator>
#
# include "preview/core.h"
#
# /* TODO: Not use this when defining views::iterator */
# if PREVIEW_CXX_VERSION >= 20 && (PREVIEW_ANDROID == 0 || (defined(PREVIEW_NDK_VERSION_MAJOR) && PREVIEW_NDK_VERSION_MAJOR >= 26))
#   define PREVIEW_STD_HAVE_CXX20_ITERATOR 1
# else
#   define PREVIEW_STD_HAVE_CXX20_ITERATOR 0
# endif

// TODO: Test using template
//namespace preview {
//namespace detail {
//
//template<typename T>
//struct cxx20_iterator_tester {
//  using difference_type = int;
//  using value_type = int;
//  using reference = int&;
//  using iterator_category = std::input_iterator_tag;
//};
//
//} // namespace preview
//} // namespace preview

# endif // PREVIEW_ITERATOR_DETAIL_STD_CHECK_H_
