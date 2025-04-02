//
// Created by yonggyulee on 2024. 10. 27.
//

#ifndef PREVIEW_MDSPAN_LAYOUT_STRIDE_H_
#define PREVIEW_MDSPAN_LAYOUT_STRIDE_H_

#include <array>
#include <utility>

#include "preview/__core/maybe_unused.h"
#include "preview/__core/cxx_version.h"
#include "preview/__array/to_array.h"
#include "preview/__mdspan/ebo_array.h"
#include "preview/__mdspan/layout_forward_declare.h"
#include "preview/__mdspan/extents.h"
#include "preview/span.h"
#include "preview/__type_traits/bool_constant.h"
#include "preview/__utility/as_const.h"
#include "preview/__utility/compressed_pair.h"
#include "preview/__utility/in_place.h"
#include "preview/__utility/integer_sequence.h"
#include "preview/__utility/cxx20_rel_ops.h"

namespace preview {
namespace detail {

template<typename Extents, typename StridedLayoutMapping>
using layout_stride_mapping_implicit =
    conjunction<
        std::is_convertible<typename StridedLayoutMapping::extents_type, Extents>,
        disjunction<
            detail::is_mapping_of<layout_left, StridedLayoutMapping>,
            detail::is_mapping_of<layout_right, StridedLayoutMapping>,
            detail::is_layout_left_padded_mapping_of<StridedLayoutMapping>,
            detail::is_layout_right_padded_mapping_of<StridedLayoutMapping>,
            detail::is_mapping_of<layout_stride, StridedLayoutMapping>>
    >;

template<typename IndexType, std::size_t... Extents, std::size_t... r>
constexpr IndexType layout_stride_required_span_size_default(
    const extents<IndexType, Extents...>& e,
    const std::array<IndexType, sizeof...(Extents)>& strides,
    PREVIEW_MAYBE_UNUSED std::integer_sequence<std::size_t, r...>) noexcept
{
#if PREVIEW_CXX_VERSION < 17
  using extents_type = extents<IndexType, Extents...>;
  using rank_type = typename extents_type::rank_type;

  IndexType size = 1;
  for (rank_type i = 0; i < extents_type::rank(); ++i) {
    size += (e.extent(i) - 1) * extents_helper::index_cast<extents_type>(strides[i]);
  }

  return size;
#else
  return (1 + ... + ((e.extent(r) - 1) * strides[r]));
#endif
}

template<typename IndexType>
constexpr IndexType
layout_stride_required_span_size(const extents<IndexType>&, const std::array<IndexType, 0>&) noexcept {
  return 1;
}

// The size of the multidimensional index space is statically 0
template<typename IndexType, std::size_t... Extents, std::enable_if_t<
    (integer_sequence_count<std::index_sequence<Extents...>, 0>::value > 0)
, int> = 0>
constexpr IndexType
layout_stride_required_span_size(const extents<IndexType, Extents...>&, const std::array<IndexType, sizeof...(Extents)>&) noexcept {
  return 0;
}

// The size of the multidimensional index space may be dynamically 0 (contains dynamic extents)
template<typename IndexType, std::size_t... Extents, std::enable_if_t<
    (integer_sequence_count<std::index_sequence<Extents...>, 0>::value == 0) &&
    (extents<IndexType, Extents...>::rank_dynamic() > 0)
, int> = 0>
constexpr IndexType layout_stride_required_span_size(
    const extents<IndexType, Extents...>& e,
    const std::array<IndexType, sizeof...(Extents)>& strides) noexcept
{
  using ExtentsType = extents<IndexType, Extents...>;
  using rank_type = typename ExtentsType::rank_type;
  constexpr rank_type rd = ExtentsType::rank_dynamic();
  for (rank_type r = 0; r < rd; ++r) {
    if (extents_helper::extent_dynamic(e, r) == 0) {
      return 0;
    }
  }

  return preview::detail::layout_stride_required_span_size_default(
      e, strides, std::make_integer_sequence<rank_type, ExtentsType::rank()>{});
}

// The size of the multidimensional index space is statically not 0
template<typename IndexType, std::size_t... Extents, std::enable_if_t<
    (integer_sequence_count<std::index_sequence<Extents...>, 0>::value == 0) &&
    (extents<IndexType, Extents...>::rank_dynamic() == 0)
, int> = 0>
constexpr IndexType
layout_stride_required_span_size(const extents<IndexType, Extents...>& e, const std::array<IndexType, sizeof...(Extents)>& strides) noexcept {
  using ExtentsType = extents<IndexType, Extents...>;
  using rank_type = typename ExtentsType::rank_type;

  return preview::detail::layout_stride_required_span_size_default(
      e, strides, std::make_integer_sequence<rank_type, ExtentsType::rank()>{});
}

template<typename T, std::size_t N>
using layout_stride_stride_base = std::conditional_t<N == 0, empty_array<T>, std::array<T, N>>;

}; // namespace detail

// TODO: Optimize zero-sized array

template<typename Extents>
class layout_stride::mapping
    : private detail::layout_stride_stride_base<typename Extents::index_type, Extents::rank()>
{
  static_assert(detail::is_extents<Extents>::value, "Extents must be a specialization of extents");
  static_assert(detail::extents_helper::index_space_is_representable<Extents>(),
                "index space Extents() must be representable as a value of type typename Extents::index_type");
 public:

  using extents_type = Extents;
  using index_type   = typename extents_type::index_type;
  using size_type    = typename extents_type::size_type;
  using rank_type    = typename extents_type::rank_type;
  using layout_type  = layout_stride;

 private:
  using stride_base = detail::layout_stride_stride_base<index_type, extents_type::rank()>;
  static constexpr rank_type rank_ = extents_type::rank();

  template<rank_type... d>
  constexpr explicit mapping(in_place_t, std::integer_sequence<rank_type, d...>)
      : stride_base{layout_right::mapping<extents_type>().stride(d)...}
      , extents_(extents_type()) {}

  template<typename SpanOrArray, rank_type... d>
  constexpr explicit mapping(in_place_t, const extents_type& e, SpanOrArray&& s, std::integer_sequence<rank_type, d...>)
      : stride_base{static_cast<index_type>(preview::as_const(s[d]))...}
      , extents_(e) {}

  template<typename StridedLayoutMapping, rank_type... d>
  constexpr explicit mapping(in_place_t, const StridedLayoutMapping& other, std::integer_sequence<rank_type, d...>)
      : stride_base{static_cast<index_type>(other.stride(d))...}
      , extents_(other.extents()) {}

 public:
  constexpr mapping() noexcept
      : mapping(in_place, std::make_integer_sequence<rank_type, rank_>{}) {}

  constexpr mapping(const mapping&) noexcept = default;

  template<typename OtherIndexType, std::enable_if_t<conjunction<
      std::is_convertible<const OtherIndexType&, index_type>,
      std::is_nothrow_constructible<index_type, const OtherIndexType&>
  >::value, int> = 0>
  constexpr mapping(const extents_type& e, span<OtherIndexType, rank_> s) noexcept
      : mapping(in_place, e, s, std::make_integer_sequence<rank_type, rank_>{}) {}

  template<typename OtherIndexType, std::enable_if_t<conjunction<
      std::is_convertible<const OtherIndexType&, index_type>,
      std::is_nothrow_constructible<index_type, const OtherIndexType&>
  >::value, int> = 0>
  constexpr mapping(const extents_type& e, const std::array<OtherIndexType, rank_>& s) noexcept
      : mapping(in_place, e, s, std::make_integer_sequence<rank_type, rank_>{}) {}

  template<typename StridedLayoutMapping, std::enable_if_t<conjunction<
      detail::layout_mapping_alike<StridedLayoutMapping>,
      std::is_constructible<extents_type, typename StridedLayoutMapping::extents_type>,
      // explicit(true)
      negation<detail::layout_stride_mapping_implicit<extents_type, StridedLayoutMapping>>
  >::value, int> = 0>
  constexpr explicit mapping(const StridedLayoutMapping& other) noexcept
      : mapping(in_place, other, std::make_integer_sequence<rank_type, rank_>{}) {}

  template<typename StridedLayoutMapping, std::enable_if_t<conjunction<
      detail::layout_mapping_alike<StridedLayoutMapping>,
      std::is_constructible<extents_type, typename StridedLayoutMapping::extents_type>,
      // explicit(false)
      detail::layout_stride_mapping_implicit<extents_type, StridedLayoutMapping>
  >::value, int> = 0>
  constexpr mapping(const StridedLayoutMapping& other) noexcept
      : mapping(in_place, other, std::make_integer_sequence<rank_type, rank_>{}) {}

  constexpr mapping& operator=(const mapping&) noexcept = default;

  constexpr const extents_type& extents() const noexcept { return extents_; }
  constexpr std::array<index_type, rank_> strides() const noexcept {
    return strides_impl(bool_constant<(rank_ > 0)>{});
  }

  constexpr index_type required_span_size() const noexcept {
    return preview::detail::layout_stride_required_span_size(extents(), strides());
  }

#if PREVIEW_CXX_VERSION < 17
  template<bool B = (extents_type::rank() == 0), std::enable_if_t<B, int> = 0>
  constexpr index_type operator()() const noexcept {
    return 0;
  }

  template<typename... Indices, std::enable_if_t<conjunction_v<
      bool_constant<(extents_type::rank() != 0)>,
      bool_constant<(sizeof...(Indices) == rank_)>,
      std::is_convertible<Indices, index_type>...,
      std::is_nothrow_constructible<index_type, Indices>...
  >, int> = 0>
  constexpr index_type operator()(Indices... i) const noexcept {
    return call(preview::to_array({static_cast<index_type>(std::move(i))...}));
  }
#else
  template<typename... Indices, std::enable_if_t<conjunction_v<
      bool_constant<(sizeof...(Indices) == rank_)>,
      std::is_convertible<Indices, index_type>...,
      std::is_nothrow_constructible<index_type, Indices>...
  >, int> = 0>
  constexpr index_type operator()(Indices... i) const noexcept {
    return call(std::index_sequence_for<Indices...>{}, static_cast<index_type>(std::move(i))...);
  }
#endif

  static constexpr bool is_always_unique() noexcept { return true; }
  static constexpr bool is_always_exhaustive() noexcept { return false; }
  static constexpr bool is_always_strided() noexcept { return true; }

  static constexpr bool is_unique() noexcept { return true; }
  constexpr bool is_exhaustive() const noexcept {
    return is_exhaustive_impl(std::integral_constant<rank_type, rank_>{});
  }
  static constexpr bool is_strided() noexcept { return true; }

  constexpr index_type stride(rank_type i) const noexcept {
#if PREVIEW_CXX_VERSION < 17
    // std::array::operator[] is not constexpr in C++17
    auto s = strides();
    return preview::as_const(s)[i];
#else
    return strides()[i];
#endif
  }

  template<class OtherMapping, std::enable_if_t<conjunction_v<
      detail::layout_mapping_alike<OtherMapping>,
      bool_constant<(rank_ == OtherMapping::extents_type::rank())>,
      bool_constant<OtherMapping::is_always_strided()>
#if !PREVIEW_CONFORM_CXX20_STANDARD
      , negation<is_specialization<std::decay_t<OtherMapping>, layout_stride::mapping>>
#endif
  >, int> = 0>
  friend constexpr bool operator==(const mapping& x, const OtherMapping& y) noexcept {
    return x.extents() == y.extents()
        && offset(y) == 0
        && x.equal_stride(y, std::make_integer_sequence<rank_type, rank_>{});
  }

#if !PREVIEW_CONFORM_CXX20_STANDARD
  template<typename OtherExtents, std::enable_if_t<(rank_ == OtherExtents::rank()), int> = 0>
  friend constexpr bool operator==(const mapping& x, const mapping<OtherExtents>& y) noexcept {
    return x.extents() == y.extents()
        && offset(y) == 0
        && x.equal_stride(y, std::make_integer_sequence<rank_type, rank_>{});
  }

  template<class OtherMapping, std::enable_if_t<conjunction_v<
      detail::layout_mapping_alike<OtherMapping>,
      bool_constant<(rank_ == OtherMapping::extents_type::rank())>,
      bool_constant<OtherMapping::is_always_strided()>,
      negation<is_specialization<OtherMapping, layout_stride::mapping>>
  >, int> = 0>
  friend constexpr bool operator==(const OtherMapping& x, const mapping& y) noexcept {
    return y == x;
  }

  template<class OtherMapping, std::enable_if_t<conjunction_v<
      detail::layout_mapping_alike<OtherMapping>,
      bool_constant<(rank_ == OtherMapping::extents_type::rank())>,
      bool_constant<OtherMapping::is_always_strided()>,
      negation<is_specialization<OtherMapping, layout_stride::mapping>>
  >, int> = 0>
  friend constexpr bool operator!=(const mapping& x, const OtherMapping& y) noexcept {
    return !(x == y);
  }

  template<class OtherMapping, std::enable_if_t<conjunction_v<
      detail::layout_mapping_alike<OtherMapping>,
      bool_constant<(rank_ == OtherMapping::extents_type::rank())>,
      bool_constant<OtherMapping::is_always_strided()>,
      negation<is_specialization<OtherMapping, layout_stride::mapping>>
  >, int> = 0>
  friend constexpr bool operator!=(const OtherMapping& x, const mapping& y) noexcept {
    return !(y == x);
  }

  template<typename OtherExtents, std::enable_if_t<(rank_ == OtherExtents::rank()), int> = 0>
  friend constexpr bool operator!=(const mapping& x, const mapping<OtherExtents>& y) noexcept {
    return !(x == y);
  }
#endif

 private:
  constexpr stride_base& as_stride_base() noexcept { return static_cast<stride_base&>(*this); }
  constexpr const stride_base& as_stride_base() const noexcept { return static_cast<const stride_base&>(*this); }

  constexpr std::array<index_type, rank_> strides_impl (std::true_type) const noexcept { return as_stride_base(); }
  constexpr std::array<index_type, 0> strides_impl (std::false_type) const noexcept { return {}; }

#if PREVIEW_CXX_VERSION < 17
  template<std::size_t Rank>
  constexpr index_type call(const std::array<index_type, Rank>& indices) const noexcept {
    index_type result = 0;
    for (std::size_t r = 0; r < Rank; ++r) {
      result += indices[r] * stride(r);
    }
    return result;
  }
  constexpr index_type call(const std::array<index_type, 1>& indices) const noexcept {
    return indices[0] * stride(0);
  }
  constexpr index_type call(const std::array<index_type, 2>& indices) const noexcept {
    return indices[0] * stride(0) + indices[1] * stride(1);
  }
  constexpr index_type call(const std::array<index_type, 3>& indices) const noexcept {
    return indices[0] * stride(0) + indices[1] * stride(1) + indices[2] * stride(2);
  }
  constexpr index_type call(const std::array<index_type, 4>& indices) const noexcept {
    return indices[0] * stride(0) + indices[1] * stride(1) + indices[2] * stride(2) + indices[3] * stride(3);
  }
#else
  template<typename... Indices, std::size_t... P>
  constexpr index_type call(std::index_sequence<P...>, Indices... i) const noexcept {
    return ((i * stride(P)) + ... + 0);
  }
#endif

  constexpr bool is_exhaustive_impl(std::integral_constant<rank_type, 0>) const noexcept {
    return true;
  }

  // stride(p0) == 1
  // stride(p1) == stride(p0) * extent(p0)
  //            == 1 * extent(1)
  // stride(p2) == stride(p1) * extent(p1)
  //            == extent(1)  * extent(extent(1))
  // stride(p3) == stride(p2) * extent(p2)
  //            == { extent(1)  * extent(extent(1)) } * extent({ extent(1)  * extent(extent(1)) })
  // ...
  // stride(pi) == stride(p(i-1)) * extent(p(i-1))

  constexpr bool is_exhaustive_impl(std::integral_constant<rank_type, 1>) const noexcept {
    index_type span_size = required_span_size();
    if (span_size == static_cast<index_type>(0)) {
      return as_stride_base()[0] == 1;
    } else {
      return required_span_size() == detail::extents_helper::fwd_prod_of_extents(extents(), rank_);
    }
  }

  template<rank_type Rank>
  constexpr bool is_exhaustive_impl(std::integral_constant<rank_type, Rank>) const noexcept {
    index_type span_size = required_span_size();
    if (span_size == static_cast<index_type>(0)) {
      rank_type rank_largest = 0;
      for (rank_type r = 1; r < Rank; r++)
        if (as_stride_base()[r] > as_stride_base()[rank_largest])
          rank_largest = r;
      for (rank_type r = 0; r < Rank; r++)
        if (extents().extent(r) == 0 && r != rank_largest)
          return false;
      return true;
    } else {
      return required_span_size() == detail::extents_helper::fwd_prod_of_extents(extents(), rank_);
    }
  }

  template<typename StridedMapping, std::enable_if_t<StridedMapping::extents_type::rank() == 0, int> = 0>
  static constexpr index_type offset(const StridedMapping& m) noexcept {
    return static_cast<index_type>(m());
  }
  template<typename StridedMapping, std::enable_if_t<StridedMapping::extents_type::rank() != 0, int> = 0>
  static constexpr index_type offset(const StridedMapping& m) noexcept {
    if (m.required_span_size() == static_cast<typename StridedMapping::index_type>(0)) {
      return 0;
    } else {
      return offset_default(m, std::make_index_sequence<rank_>{});
    }
  }
  template<typename StridedMapping, std::size_t... i>
  static constexpr index_type offset_default(const StridedMapping& m, std::index_sequence<i...>) noexcept {
    return static_cast<index_type>(m(((void)i, 0)...));
  }

  template<typename OtherMapping, rank_type... R>
  constexpr bool equal_stride(const OtherMapping& y, std::integer_sequence<rank_type, R...>) const noexcept {
    static_assert(rank_ == OtherMapping::extents_type::rank(), "Rank mismatch");
    using namespace preview::rel_ops;
#if PREVIEW_CXX_VERSION < 17
    for (rank_type r = 0; r < rank_; ++r) {
      if (stride(r) != y.stride(r)) {
        return false;
      }
    }
    return true;
#else
    return ((stride(R) == y.stride(R)) && ... && true);
#endif
  }

  template<typename OtherMapping>
  constexpr bool equal_stride(const OtherMapping&, std::integer_sequence<rank_type>) const noexcept {
    static_assert(rank_ == OtherMapping::extents_type::rank(), "Rank mismatch");
    return true;
  }

#if PREVIEW_CXX_VERSION >= 17
  template<typename OtherMapping>
  constexpr bool equal_to(const OtherMapping& other, std::integer_sequence<rank_type>) const noexcept {
    return extents() == other.extents()
        && offset(other) == 0;
  }

  template<typename OtherMapping, rank_type... r>
  constexpr bool equal_to(const OtherMapping& other, std::integer_sequence<rank_type, r...>) const noexcept {
    return extents() == other.extents()
        && offset(other) == 0
        && ((stride(r) == other.stride(r)) && ... && true);
  }
#endif

  Extents extents_;

// TODO: Implement submdspan_mapping
//
//  template<class... SliceSpecifiers>
//  constexpr auto submdspan-mapping-impl(SliceSpecifiers...) const
//  -> see below;
//
//  template<class... SliceSpecifiers>
//  friend constexpr auto submdspan_mapping(
//      const mapping& src, SliceSpecifiers... slices) {
//    return src.submdspan-mapping-impl(slices...);
//  }
};

} // namespace preview

#endif // PREVIEW_MDSPAN_LAYOUT_STRIDE_H_
