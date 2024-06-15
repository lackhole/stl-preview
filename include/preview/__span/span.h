//
// Created by yonggyulee on 2023/12/30.
//

#ifndef PREVIEW_SPAN_SPAN_H_
#define PREVIEW_SPAN_SPAN_H_

#include <array>
#include <cassert>
#include <cstddef>
#include <iterator>
#include <stdexcept>
#include <type_traits>

#include "preview/__iterator/contiguous_iterator.h"
#include "preview/__iterator/iter_reference_t.h"
#include "preview/__iterator/sized_sentinel_for.h"
#include "preview/__memory/to_address.h"
#include "preview/__ranges/enable_borrowed_range.h"
#include "preview/__ranges/enable_view.h"
#include "preview/__ranges/borrowed_range.h"
#include "preview/__ranges/contiguous_range.h"
#include "preview/__ranges/data.h"
#include "preview/__ranges/sized_range.h"
#include "preview/__ranges/range_reference_t.h"
#include "preview/__span/__forward_declare.h"
#include "preview/__type_traits/bool_constant.h"
#include "preview/__type_traits/conjunction.h"
#include "preview/__type_traits/negation.h"
#include "preview/__type_traits/remove_cvref.h"
#include "preview/__type_traits/type_identity.h"

namespace preview {
namespace detail {

template<typename T, std::size_t Extent>
struct static_span_storage {
  constexpr static_span_storage() noexcept : ptr_(nullptr) {}
  constexpr static_span_storage(T* ptr, std::size_t) noexcept : ptr_(ptr) {}

  T* data() const noexcept { return ptr_; }
  std::size_t size() const noexcept { return Extent; }

  T* ptr_;
};

template<typename T, std::size_t>
struct dynamic_span_storage {
  constexpr dynamic_span_storage() noexcept : ptr_(nullptr), size_(0) {}
  constexpr dynamic_span_storage(T* ptr, std::size_t size) noexcept : ptr_(ptr), size_(size) {}

  T* data() const noexcept { return ptr_; }
  std::size_t size() const noexcept { return size_; }

  T* ptr_;
  std::size_t size_;
};

template<typename T, std::size_t Extent>
using span_storage_t = std::conditional_t<
    Extent == dynamic_extent, dynamic_span_storage<T, Extent>,
    static_span_storage<T, Extent>>;

template<typename T>
struct is_span : std::false_type {};
template<typename T, std::size_t Extent>
struct is_span<span<T, Extent>> : std::true_type {};

template<typename T>
struct is_array : std::false_type {};
template<typename T, std::size_t N>
struct is_array<std::array<T, N>> : std::true_type {};

template<typename It, typename T, bool = contiguous_iterator<It>::value /* true */>
struct span_ctor_first_count : std::is_convertible<iter_reference_t<It>, T> {};
template<typename It, typename T>
struct span_ctor_first_count<It, T, false> : std::false_type {};

template<typename It, typename End, typename T, bool = conjunction<
    contiguous_iterator<It>, sized_sentinel_for<End, It> >::value /* true */>
struct span_ctor_first_last : conjunction<
    std::is_convertible<iter_reference_t<It>, T>,
    negation<std::is_convertible<End, std::size_t>> > {};
template<typename It, typename End, typename T>
struct span_ctor_first_last<It, End, T, false> : std::false_type {};

template<typename R, typename T, bool = conjunction<
      ranges::contiguous_range<R>,
      ranges::sized_range<R>,
      disjunction< ranges::borrowed_range<R>, std::is_const<T> >,
      negation< is_span<remove_cvref_t<R>> >,
      negation< is_array<remove_cvref_t<R>> >,
      negation< std::is_array<remove_cvref_t<R>> >
    >::value /* true */>
struct span_ctor_range : std::is_convertible<ranges::range_reference_t<R>, T> {};
template<typename R, typename T>
struct span_ctor_range<R, T, false> : std::false_type {};

} // namespace detail

template<typename T, std::size_t Extent>
class span : private detail::span_storage_t<T, Extent> {
 public:
  using element_type = T;
  using value_type = std::remove_cv_t<T>;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;
  using pointer = T*;
  using const_pointer = const T*;
  using reference = T&;
  using const_reference = const T&;
  using iterator = pointer;
  using reverse_iterator = std::reverse_iterator<iterator>;

 private:
  using base = detail::span_storage_t<T, Extent>;

  template<std::size_t Offset, std::size_t Count>
  using subspan_type = span<element_type,
                            (Count != dynamic_extent ? Count :
                             Extent != dynamic_extent ? Extent - Offset :
                             dynamic_extent)>;
 public:
  // (1)
  // constexpr span() noexcept;
  constexpr span() noexcept : base() {}

  // (2)
  // template< class It >
  // explicit(extent != std::dynamic_extent)
  // constexpr span( It first, size_type count );
  template<typename It, std::enable_if_t<
    conjunction<
      bool_constant<(Extent == dynamic_extent)>,
      detail::span_ctor_first_count<It, element_type>
    >::value, int> = 0>
  constexpr span(It first, size_type size)
      : base(preview::to_address(first), size) {}

  template<typename It, std::enable_if_t<
    conjunction<
      bool_constant<(Extent != dynamic_extent)>,
      detail::span_ctor_first_count<It, element_type>
    >::value, int> = 0>
  constexpr explicit span(It first, size_type size)
      : base(preview::to_address(first), size) {}

  // (3)
  // template< class It, class End >
  // explicit(extent != std::dynamic_extent)
  // constexpr span( It first, End last );
  template<typename It, typename End, std::enable_if_t<
    conjunction<
      bool_constant<(Extent == dynamic_extent)>,
      detail::span_ctor_first_last<It, End, element_type>
    >::value , int> = 0>
  constexpr span(It first, End last)
      : base(preview::to_address(first), last - first) {}

  template<typename It, typename End, std::enable_if_t<
    conjunction<
      bool_constant<(Extent != dynamic_extent)>,
      detail::span_ctor_first_last<It, End, element_type>
    >::value , int> = 0>
  constexpr explicit span(It first, End last)
      : base(preview::to_address(first), last - first) {}

  // (4)
  // template< std::size_t N >
  // constexpr span( std::type_identity_t<element_type> (&arr)[N] ) noexcept;
  template<std::size_t N, std::enable_if_t<
    conjunction<
      bool_constant<(Extent == dynamic_extent) || (Extent == N)>,
      std::is_convertible<type_identity_t<element_type>, element_type>
    >::value, int> = 0>
  constexpr span(type_identity_t<element_type> (&arr)[N]) noexcept
      : base(arr, N) {}

  // (5)
  // template< class U, std::size_t N >
  // constexpr span( std::array<U, N>& arr ) noexcept;
  template<typename U, std::size_t N, std::enable_if_t<
    conjunction<
      bool_constant<(Extent == dynamic_extent) || (Extent == N)>,
      std::is_convertible<U, element_type>
    >::value, int> = 0>
  constexpr span(std::array<U, N>& arr) noexcept
      : base(arr.data(), N) {}

  // (6)
  // template< class U, std::size_t N >
  // constexpr span( const std::array<U, N>& arr ) noexcept;
  template<typename U, std::size_t N, std::enable_if_t<
    conjunction<
      bool_constant<(Extent == dynamic_extent) || (Extent == N)>,
      std::is_convertible<const U, element_type>
    >::value, int> = 0>
  constexpr span(const std::array<U, N>& arr) noexcept
      : base(arr.data(), N) {}

  // (7)
  // template< class R >
  // explicit(extent != std::dynamic_extent)
  // constexpr span( R&& range );
  template<typename R, std::enable_if_t<
    conjunction<
      bool_constant<Extent == dynamic_extent>,
      detail::span_ctor_range<R, element_type>
  >::value, int> = 0>
  constexpr span(R&& range)
      : base(ranges::data(range),
             ranges::size(range)) {}

  template<typename R, std::enable_if_t<
    conjunction<
      bool_constant<Extent != dynamic_extent>,
      detail::span_ctor_range<R, element_type>
  >::value, int> = 0>
  constexpr explicit span(R&& range)
      : base(ranges::data(range),
             ranges::size(range)) {}

  // (8) (C++ 26)
  // explicit(extent != std::dynamic_extent)
  // constexpr span( std::initializer_list<value_type> il ) noexcept;
  template<typename E = element_type, std::enable_if_t<conjunction<
      bool_constant<Extent == dynamic_extent>,
      std::is_const<E>
  >::value, int> = 0>
  constexpr span(std::initializer_list<value_type> il) noexcept
      : base(il.begin(), il.size()) {}

  template<typename E = element_type, std::enable_if_t<conjunction<
      bool_constant<Extent != dynamic_extent>,
      std::is_const<E>
  >::value, int> = 0>
  constexpr explicit span(std::initializer_list<value_type> il) noexcept
      : base(il.begin(), il.size()) {}

  // (9)
  // template< class U, std::size_t N >
  // explicit(extent != std::dynamic_extent && N == std::dynamic_extent)
  // constexpr span( const std::span<U, N>& source ) noexcept;
  template<typename U, std::size_t N, std::enable_if_t<
      conjunction<
        bool_constant<!(Extent != dynamic_extent && N == dynamic_extent)>,
        bool_constant<(
          Extent == dynamic_extent ||
          N == dynamic_extent ||
          N == Extent)>,
        std::is_convertible<U, element_type>
      >::value, int> = 0>
  constexpr span(const span<U, N>& source) noexcept : base(source.data(), source.size()) {}

  template<typename U, std::size_t N, std::enable_if_t<
      conjunction<
        bool_constant<(Extent != dynamic_extent && N == dynamic_extent)>,
        bool_constant<(
          Extent == dynamic_extent ||
          N == dynamic_extent ||
          N == Extent)>,
        std::is_convertible<U, element_type>
      >::value, int> = 0>
  constexpr explicit span(const span<U, N>& source) noexcept : base(source.data(), source.size()) {}

  // (10)
  // constexpr span( const span& other ) noexcept = default;
  constexpr span(const span& other) noexcept = default;

  constexpr iterator begin() const noexcept { return iterator(data()); }
  constexpr iterator end() const noexcept { return iterator(data() + size()); }
  constexpr reverse_iterator rbegin() const noexcept { return reverse_iterator(data() + size()); }
  constexpr reverse_iterator rend() const noexcept { return reverse_iterator(data()); }

  constexpr reference front() const { return *begin(); }
  constexpr reference back() const { return *(end() - 1); }

  constexpr reference at(size_type pos) const {
    if (pos >= size()) {
      throw std::out_of_range("preview::span: out of range");
    }
    return *(data() + pos);
  }

  constexpr reference operator[](size_type idx) const { return data()[idx]; }

  constexpr pointer data() const noexcept { return base::data(); }

  constexpr size_type size() const noexcept { return base::size(); }
  constexpr size_type size_bytes() const noexcept { return size() * sizeof(element_type); }

  constexpr bool empty() const noexcept { return size() == 0; }

  template<std::size_t Count, std::enable_if_t<(Count <= Extent), int> = 0>
  constexpr span<element_type, Count> first() const {
    return span<element_type, Count>(data(), Count);
  }
  constexpr span<element_type, dynamic_extent> first(size_type Count) const {
    return span<element_type, dynamic_extent>(data(), Count);
  }

  template<std::size_t Count, std::enable_if_t<(Count <= Extent), int> = 0>
  constexpr span<element_type, Count> last() const {
    return span<element_type, Count>(data() + size() - Count, Count);
  }
  constexpr span<element_type, dynamic_extent> last(size_type Count) const {
    return span<element_type, dynamic_extent>(data() + size() - Count, Count);
  }

  template<std::size_t Offset, std::size_t Count = dynamic_extent, std::enable_if_t<
    (Offset <= Extent) && (Count == dynamic_extent || Count <= (Extent - Offset)), int> = 0>
  constexpr subspan_type<Offset, Count> subspan() const {
    return subspan_type<Offset, Count>(
      data() + Offset,
      (Count == dynamic_extent ? size() - Offset : Count));
  };
  constexpr span<element_type, dynamic_extent> subspan(size_type Offset, size_type Count) const {
    return span<element_type, dynamic_extent>(
      data() + Offset,
      (Count == dynamic_extent ? size() - Offset : Count));
  }
};

template<typename T, std::size_t Extent>
struct ranges::enable_borrowed_range<span<T, Extent>> : std::true_type {};

template<typename T, std::size_t Extent>
struct ranges::enable_view<span<T, Extent>> : std::true_type {};

#if __cplusplus >= 201703L

template<typename It, typename EndOrSize, std::enable_if_t<contiguous_iterator<It>::value, int> = 0>
span(It, EndOrSize) -> span<std::remove_reference_t<iter_reference_t<It>>>;

template<typename T, std::size_t N>
span(T (&)[N]) -> span<T, N>;

template<typename T, std::size_t N>
span(std::array<T, N>&) -> span<T, N>;

template<typename T, std::size_t N>
span(const std::array<T, N>&) -> span<const T, N>;

template<typename R>
span(R&&) -> span<std::remove_reference_t<ranges::range_reference_t<R>>>;

#endif

} // namespace preview

#endif // PREVIEW_SPAN_SPAN_H_
