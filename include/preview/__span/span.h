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

#include "preview/__core/nodiscard.h"
#include "preview/__core/cxx_version.h"
#include "preview/__iterator/basic_const_iterator.h"
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

#if PREVIEW_CXX_VERSION >= 20
#include <span>
#endif

namespace preview {
namespace detail {

struct static_constexpr_value_tester {
  template<typename T>
  static constexpr auto test(int)
      -> decltype(std::integral_constant<decltype(T::value), T::value>{}, std::true_type{});
  template<typename T>
  static constexpr auto test(...) -> std::false_type;
};

struct static_constexpr_operator_tester {
  template<typename T>
  static constexpr auto test(int)
      -> decltype(std::integral_constant<decltype(T::value), (T())>{}, std::true_type{});
  template<typename T>
  static constexpr auto test(...) -> std::false_type;
};

template<typename T>
struct has_static_constexpr_value
    : conjunction<
        decltype(static_constexpr_value_tester::test<std::remove_reference_t<T>>(0)),
        decltype(static_constexpr_operator_tester::test<std::remove_reference_t<T>>(0))
    > {};

template<typename T, bool /* false */ = has_static_constexpr_value<T>::value>
struct integral_constant_like : std::false_type {};
template<typename T>
struct integral_constant_like<T, true>
    : conjunction<
        std::is_integral<decltype(T::value)>,
        negation<std::is_same<bool, std::remove_const_t<decltype(T::value)>>>,
        convertible_to<T, decltype(T::value)>,
        equality_comparable_with<T, decltype(T::value)>,
        bool_constant<(T() == T::value)>,
        bool_constant<(static_cast<decltype(T::value)>(T()) == T::value)>
    > {};

template<typename T, bool = /* false */ integral_constant_like<T>::value>
struct maybe_static_ext : std::integral_constant<std::size_t, dynamic_extent> {};

template<typename T>
struct maybe_static_ext<T, true> : std::integral_constant<std::size_t, T::value> {};

template<typename T, std::size_t Extent>
struct static_span_storage {
  constexpr static_span_storage() noexcept : ptr_(nullptr) {}
  constexpr static_span_storage(T* ptr, std::size_t) noexcept : ptr_(ptr) {}

  constexpr T* data() const noexcept { return ptr_; }

  PREVIEW_NODISCARD
  constexpr std::size_t size() const noexcept { return Extent; }

  T* ptr_;
};

template<typename T>
struct dynamic_span_storage {
  constexpr dynamic_span_storage() noexcept : ptr_(nullptr), size_(0) {}
  constexpr dynamic_span_storage(T* ptr, std::size_t size) noexcept : ptr_(ptr), size_(size) {}

  constexpr T* data() const noexcept { return ptr_; }

  PREVIEW_NODISCARD
  constexpr std::size_t size() const noexcept { return size_; }

  T* ptr_;
  std::size_t size_;
};

template<typename T, std::size_t Extent>
using span_storage_t = std::conditional_t<
    Extent == dynamic_extent, dynamic_span_storage<T>,
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
struct span_ctor_first_count
    : std::is_convertible<
        std::remove_reference_t<iter_reference_t<It>>(*)[],
        T(*)[]
    > {};
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
struct span_ctor_range : std::is_convertible<std::remove_reference_t<ranges::range_reference_t<R>>(*)[], T(*)[]> {};
template<typename R, typename T>
struct span_ctor_range<R, T, false> : std::false_type {};

} // namespace detail

template<typename T, std::size_t Extent>
class span : private detail::span_storage_t<T, Extent> {
  using base = detail::span_storage_t<T, Extent>;

 public:
  using element_type = T;
  using value_type = std::remove_cv_t<T>;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;
  using pointer = element_type*;
  using const_pointer = const element_type*;
  using reference = element_type&;
  using const_reference = const element_type&;
  // TODO: Custom iterator?
  using iterator = pointer;
  using const_iterator = preview::const_iterator<iterator>;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = preview::const_iterator<reverse_iterator>;
  static constexpr size_type extent = Extent;

  // (1)
  // constexpr span() noexcept;
  constexpr span() noexcept : base() {}

  // (2)
  // template< class It >
  // explicit(extent != std::dynamic_extent)
  // constexpr span( It first, size_type count );
  template<typename It, std::enable_if_t<conjunction<
      bool_constant<(Extent == dynamic_extent)>,
      detail::span_ctor_first_count<It, element_type>
  >::value, int> = 0>
  constexpr span(It first, size_type size)
      : base(preview::to_address(first), size) {}

  template<typename It, std::enable_if_t<conjunction<
      bool_constant<(Extent != dynamic_extent)>,
      detail::span_ctor_first_count<It, element_type>
  >::value, int> = 0>
  constexpr explicit span(It first, size_type size)
      : base(preview::to_address(first), size) {}

  // (3)
  // template< class It, class End >
  // explicit(extent != std::dynamic_extent)
  // constexpr span( It first, End last );
  template<typename It, typename End, std::enable_if_t<conjunction<
      bool_constant<(Extent == dynamic_extent)>,
      detail::span_ctor_first_last<It, End, element_type>
  >::value , int> = 0>
  constexpr span(It first, End last)
      : base(preview::to_address(first), last - first) {}

  template<typename It, typename End, std::enable_if_t<conjunction<
      bool_constant<(Extent != dynamic_extent)>,
      detail::span_ctor_first_last<It, End, element_type>
  >::value , int> = 0>
  constexpr explicit span(It first, End last)
      : base(preview::to_address(first), last - first) {}

  // (4)
  // template< std::size_t N >
  // constexpr span( std::type_identity_t<element_type> (&arr)[N] ) noexcept;
  template<std::size_t N, std::enable_if_t<(Extent == dynamic_extent) || (Extent == N), int> = 0>
  constexpr span(type_identity_t<element_type> (&arr)[N]) noexcept
      : base(arr, N) {}

  // (5)
  // template< class U, std::size_t N >
  // constexpr span( std::array<U, N>& arr ) noexcept;
  template<typename U, std::size_t N, std::enable_if_t<conjunction<
      bool_constant<(Extent == dynamic_extent) || (Extent == N)>,
      std::is_convertible<U(*)[], element_type(*)[]>
  >::value, int> = 0>
  constexpr span(std::array<U, N>& arr) noexcept
      : base(arr.data(), N) {}

  // (6)
  // template< class U, std::size_t N >
  // constexpr span( const std::array<U, N>& arr ) noexcept;
  template<typename U, std::size_t N, std::enable_if_t<conjunction<
      bool_constant<(Extent == dynamic_extent) || (Extent == N)>,
      std::is_convertible<const U(*)[], element_type(*)[]>
  >::value, int> = 0>
  constexpr span(const std::array<U, N>& arr) noexcept
      : base(arr.data(), N) {}

  // (7)
  // template< class R >
  // explicit(extent != std::dynamic_extent)
  // constexpr span( R&& range );
  template<typename R, std::enable_if_t<conjunction<
      bool_constant<Extent == dynamic_extent>,
      detail::span_ctor_range<R, element_type>
  >::value, int> = 0>
  constexpr span(R&& range)
      : base(ranges::data(range), ranges::size(range)) {}

  template<typename R, std::enable_if_t<conjunction<
      bool_constant<Extent != dynamic_extent>,
      detail::span_ctor_range<R, element_type>
  >::value, int> = 0>
  constexpr explicit span(R&& range)
      : base(ranges::data(range), ranges::size(range)) {}

  // (8) (C++ 26)
  // explicit(extent != std::dynamic_extent)
  // constexpr span( std::initializer_list<value_type> il ) noexcept;
  template<typename Dummy = void, std::enable_if_t<conjunction<std::is_void<Dummy>,
      bool_constant<Extent == dynamic_extent>,
      std::is_const<element_type>
  >::value, int> = 0>
  constexpr span(std::initializer_list<value_type> il) noexcept
      : base(il.begin(), il.size()) {}

  template<typename Dummy = void, std::enable_if_t<conjunction<std::is_void<Dummy>,
      bool_constant<Extent != dynamic_extent>,
      std::is_const<element_type>
  >::value, int> = 0>
  constexpr explicit span(std::initializer_list<value_type> il) noexcept
      : base(il.begin(), il.size()) {}

  // (9)
  // template< class U, std::size_t N >
  // explicit(extent != std::dynamic_extent && N == std::dynamic_extent)
  // constexpr span( const std::span<U, N>& source ) noexcept;
  template<typename U, std::size_t OtherExtent, std::enable_if_t<conjunction<
      bool_constant<(
        Extent == dynamic_extent ||
        OtherExtent == dynamic_extent ||
        Extent == OtherExtent)>,
      std::is_convertible<U(*)[], element_type(*)[]>,
      // explicit(false)
      bool_constant<!(Extent != dynamic_extent && OtherExtent == dynamic_extent)>
  >::value, int> = 0>
  constexpr span(const span<U, OtherExtent>& source) noexcept
      : base(source.data(), source.size()) {}

  template<typename U, std::size_t OtherExtent, std::enable_if_t<conjunction<
      bool_constant<(
          Extent == dynamic_extent ||
          OtherExtent == dynamic_extent ||
          Extent == OtherExtent)>,
      std::is_convertible<U(*)[], element_type(*)[]>,
      // explicit(true)
      bool_constant<(Extent != dynamic_extent && OtherExtent == dynamic_extent)>
  >::value, int> = 0>
  constexpr explicit span(const span<U, OtherExtent>& source) noexcept
      : base(source.data(), source.size()) {}

  // (10)
  // constexpr span( const span& other ) noexcept = default;
  constexpr span(const span& other) noexcept = default;

#if PREVIEW_CXX_VERSION >= 20
  template<typename U, std::size_t OtherExtent, std::enable_if_t<conjunction<
      bool_constant<(
          Extent == dynamic_extent ||
          OtherExtent == dynamic_extent ||
          Extent == OtherExtent)>,
      std::is_convertible<U(*)[], element_type(*)[]>,
      // explicit(true)
      bool_constant<(Extent != dynamic_extent && OtherExtent == dynamic_extent)>
  >::value, int> = 0>
  constexpr explicit span(const std::span<U, OtherExtent>& source) noexcept
      : base(source.data(), source.size()) {}

  template<typename U, std::size_t OtherExtent, std::enable_if_t<conjunction<
      bool_constant<(
          Extent == dynamic_extent ||
          OtherExtent == dynamic_extent ||
          Extent == OtherExtent)>,
      std::is_convertible<U(*)[], element_type(*)[]>,
      // explicit(false)
      bool_constant<!(Extent != dynamic_extent && OtherExtent == dynamic_extent)>
  >::value, int> = 0>
  constexpr span(const std::span<U, OtherExtent>& source) noexcept
      : base(source.data(), source.size()) {}

  template<typename U, std::size_t OtherExtent, std::enable_if_t<conjunction<
      bool_constant<(
          Extent == dynamic_extent ||
          OtherExtent == dynamic_extent ||
          Extent == OtherExtent)>,
      std::is_convertible<U(*)[], element_type(*)[]>,
      // explicit(true)
      bool_constant<(OtherExtent != dynamic_extent && Extent == dynamic_extent)>
  >::value, int> = 0>
  constexpr explicit operator std::span<U, OtherExtent>() const noexcept {
    return std::span<U, OtherExtent>(data(), size());
  }

  template<typename U, std::size_t OtherExtent, std::enable_if_t<conjunction<
      bool_constant<(
          Extent == dynamic_extent ||
          OtherExtent == dynamic_extent ||
          Extent == OtherExtent)>,
      std::is_convertible<U(*)[], element_type(*)[]>,
      // explicit(false)
      bool_constant<!(OtherExtent != dynamic_extent && Extent == dynamic_extent)>
  >::value, int> = 0>
  constexpr operator std::span<U, OtherExtent>() const noexcept {
    return std::span<U, OtherExtent>(data(), size());
  }
#endif

  constexpr iterator begin() const noexcept { return iterator{data()}; }
  constexpr iterator end() const noexcept { return iterator{data() + size()}; }

  constexpr const_iterator cbegin() const noexcept { return begin(); }
  constexpr const_iterator cend() const noexcept { return end(); }

  constexpr reverse_iterator rbegin() const noexcept { return reverse_iterator(end()); }
  constexpr reverse_iterator rend() const noexcept { return reverse_iterator(begin()); }

  constexpr const_reverse_iterator crbegin() const noexcept { return rbegin(); }
  constexpr const_reverse_iterator crend() const noexcept { return rend(); }

  constexpr reference front() const { return *data(); }
  constexpr reference back() const { return *(data() + (size() - 1)); }

  constexpr reference at(size_type pos) const {
    if (pos >= size()) {
      throw std::out_of_range("preview::span: out of range");
    }
    return *(data() + pos);
  }

  constexpr reference operator[](size_type idx) const { return *(data() +idx); }

  constexpr pointer data() const noexcept { return base::data(); }

  constexpr size_type size() const noexcept { return base::size(); }
  constexpr size_type size_bytes() const noexcept { return size() * sizeof(element_type); }

  constexpr bool empty() const noexcept { return size() == 0; }

  template<std::size_t Count, std::enable_if_t<(Count <= Extent), int> = 0>
  constexpr span<element_type, Count> first() const {
    return span<element_type, Count>(data(), Count);
  }
  constexpr span<element_type, dynamic_extent> first(size_type count) const {
    return span<element_type, dynamic_extent>(data(), count);
  }

  template<std::size_t Count, std::enable_if_t<(Count <= Extent), int> = 0>
  constexpr span<element_type, Count> last() const {
    return span<element_type, Count>(data() + (size() - Count), Count);
  }
  constexpr span<element_type, dynamic_extent> last(size_type count) const {
    return span<element_type, dynamic_extent>(data() + (size() - count), count);
  }

  template<std::size_t Offset, std::size_t Count = dynamic_extent, std::enable_if_t<
    (Offset <= Extent) &&
    (Count == dynamic_extent || Count <= (Extent - Offset))
  , int> = 0>
  constexpr auto subspan() const {
    using subspan_type = span<element_type, (Count != dynamic_extent ? Count :
                                             Extent != dynamic_extent ? Extent - Offset :
                                             dynamic_extent)>;
    return subspan_type(data() + Offset, Count != dynamic_extent ? Count : size() - Offset);
  }
  constexpr span<element_type, dynamic_extent> subspan(size_type offset, size_type count) const {
    return span<element_type, dynamic_extent>(data() + offset, (count == dynamic_extent ? size() - offset : count));
  }
};

template<typename T, std::size_t Extent>
struct ranges::enable_view<span<T, Extent>> : std::true_type {};

#if PREVIEW_CXX_VERSION >= 17

template<typename It, typename EndOrSize, std::enable_if_t<contiguous_iterator<It>::value, int> = 0>
span(It, EndOrSize) -> span<std::remove_reference_t<iter_reference_t<It>>, detail::maybe_static_ext<EndOrSize>::value>;

template<typename T, std::size_t N>
span(T (&)[N]) -> span<T, N>;

template<typename T, std::size_t N>
span(std::array<T, N>&) -> span<T, N>;

template<typename T, std::size_t N>
span(const std::array<T, N>&) -> span<const T, N>;

template<typename R, std::enable_if_t<ranges::contiguous_range<R>::value, int> = 0>
span(R&&) -> span<std::remove_reference_t<ranges::range_reference_t<R>>>;

#endif

} // namespace preview

template<typename T, std::size_t Extent>
PREVIEW_SPECIALIZE_ENABLE_BORROWED_RANGE(preview::span<T, Extent>) = true;

#endif // PREVIEW_SPAN_SPAN_H_
