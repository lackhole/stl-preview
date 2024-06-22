# /*
#  * Created by YongGyu Lee on 2024/06/23.
#  */
#
# ifndef PREVIEW_TUPLE_TUPLE_INTEGER_SEQUENCE_H_
# define PREVIEW_TUPLE_TUPLE_INTEGER_SEQUENCE_H_
#
# include <cstddef>
# include <tuple>
# include <utility>
#
# include "preview/__type_traits/remove_cvref.h"

namespace preview {

template<typename T, typename Tuple>
using tuple_integer_sequence = std::make_integer_sequence<T, std::tuple_size<remove_cvref_t<Tuple>>::value>;

template<typename Tuple>
using tuple_index_sequence = tuple_integer_sequence<std::size_t, Tuple>;

} // namespace preview

# endif // PREVIEW_TUPLE_TUPLE_INTEGER_SEQUENCE_H_
