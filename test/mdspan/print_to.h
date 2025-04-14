//
// Created by yonggyulee on 2024. 11. 3.
//

#ifndef PREVIEW_TEST_MDSPAN_PRINT_TO_H_
#define PREVIEW_TEST_MDSPAN_PRINT_TO_H_

#include <ostream>
#include <string>
#include <type_traits>
#include <utility>

#include "preview/mdspan.h"
#include "preview/utility.h"

template<typename T>
struct TypeId {
  static std::string name() {
    using type = std::remove_cv_t<T>;

    if (std::is_same<type, char>::value) return "char";
    else if (std::is_same<type, signed char>::value) return "signed char";
    else if (std::is_same<type, unsigned char>::value) return "unsigned char";
    else if (std::is_same<type, short>::value) return "short";
    else if (std::is_same<type, unsigned short>::value) return "unsigned short";
    else if (std::is_same<type, int>::value) return "int";
    else if (std::is_same<type, unsigned>::value) return "unsigned";
    else if (std::is_same<type, unsigned int>::value) return "int";
    else if (std::is_same<type, long>::value) return "long";
    else if (std::is_same<type, unsigned long>::value) return "unsigned long";
    else if (std::is_same<type, long long>::value) return "long long";
    else if (std::is_same<type, unsigned long long>::value) return "unsigned long long";
    else if (std::is_same<type, float>::value) return "float";
    else if (std::is_same<type, double>::value) return "double";

    else if (std::is_same<type, std::size_t>::value) return "size_t";
    else if (std::is_same<type, std::ptrdiff_t>::value) return "ptrdiff_t";
  }
};

template<typename IndexType>
struct TypeId<preview::extents<IndexType>> {
  static std::string name() {
    using namespace std::literals;
    return "preview::extents<"s + TypeId<IndexType>::name() + ">";
  }
};

template<typename IndexType, std::size_t... Extents>
struct TypeId<preview::extents<IndexType, Extents...>> {
  static std::string name() {
    std::string result = "preview::extents<";
    result += TypeId<IndexType>::name();
    {
      PREVIEW_MAYBE_UNUSED int dummy[] = {
          ((result += std::string{", "} + (Extents == preview::dynamic_extent ? "dynamic_extent" : std::to_string(Extents))), 0)...
      };
    }
    result += ">";
    return result;
  }
};

template<typename Extents>
struct TypeId<preview::layout_stride::mapping<Extents>> {
  static std::string name() {
    using namespace std::literals;
    return "preview::layout_stride::mapping<"s + TypeId<Extents>::name() + ">";
  }
};

template<typename Extents>
struct TypeId<preview::layout_left::mapping<Extents>> {
  static std::string name() {
    using namespace std::literals;
    return "preview::layout_left::mapping<"s + TypeId<Extents>::name() + ">";
  }
};

template<typename Extents>
struct TypeId<preview::layout_right::mapping<Extents>> {
  static std::string name() {
    using namespace std::literals;
    return "preview::layout_right::mapping<"s + TypeId<Extents>::name() + ">";
  }
};

namespace preview {

template <typename IndexType, std::size_t... Extents, std::size_t... I>
void PrintToImpl(const preview::extents<IndexType, Extents...>& e, std::ostream* os, std::index_sequence<I...>) {
  if (e.rank() == 0) {
    *os << "{}";
    return;
  } else if (e.rank() == 1) {
    *os << "{ " << e.extent(0) << " }";
    return;
  } else {
    *os << "{";
    {
      preview::sequence_for_each(std::index_sequence<I...>{},
          [os, &e](auto i) {
            *os << " " << e.extent(i);
          }
      );
    }
    *os << " }";
  }
}

template <typename T, std::size_t N, std::size_t... I>
void PrintToImpl(const std::array<T, N>& arr, std::ostream* os, std::index_sequence<I...>) {
  if (N == 0) {
    *os << "{}";
    return;
  } else if (N == 1) {
    *os << "{ " << arr[0] << " }";
    return;
  } else {
    *os << "{";
    {
      preview::sequence_for_each(std::index_sequence<I...>{},
          [os, &arr](auto i) {
            *os << " " << arr[i];
          }
      );
    }
    *os << " }";
  }
}

template <typename IndexType, std::size_t... Extents>
void PrintTo(const preview::extents<IndexType, Extents...>& e, ::std::ostream* os) {
  *os << TypeId<preview::extents<IndexType, Extents...>>::name();
  PrintToImpl(e, os, std::make_index_sequence<sizeof...(Extents)>{});
}

template <typename Extents>
void PrintTo(const preview::layout_stride::mapping<Extents>& m, ::std::ostream* os) {
  *os << TypeId<preview::layout_stride::mapping<Extents>>::name();
  *os << ": [extents: ";
  PrintToImpl(m.extents(), os, std::make_index_sequence<Extents::rank()>{});
  *os << ", strides: ";
  PrintToImpl(m.strides(), os, std::make_index_sequence<Extents::rank()>{});
  *os << "]";
}

template <typename Extents>
void PrintTo(const preview::layout_left::mapping<Extents>& m, ::std::ostream* os) {
  *os << TypeId<std::decay_t<decltype(m)>>::name();
  *os << ": [extents: ";
  PrintToImpl(m.extents(), os, std::make_index_sequence<Extents::rank()>{});
  *os << "]";
}

template <typename Extents>
void PrintTo(const preview::layout_right::mapping<Extents>& m, ::std::ostream* os) {
  *os << TypeId<std::decay_t<decltype(m)>>::name();
  *os << ": [extents: ";
  PrintToImpl(m.extents(), os, std::make_index_sequence<Extents::rank()>{});
  *os << "]";
}

} // namespace preview

#endif // PREVIEW_TEST_MDSPAN_PRINT_TO_H_
