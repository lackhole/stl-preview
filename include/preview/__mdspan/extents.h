//
// Created by yonggyulee on 2024. 9. 27.
//

#ifndef PREVIEW_MDSPAN_EXTENTS_H_
#define PREVIEW_MDSPAN_EXTENTS_H_

#include <array>
#include <cstddef>
#include <limits>
#include <type_traits>
#include <utility>

#include "preview/core.h"
#include "preview/span.h"
#include "preview/__type_traits/bool_constant.h"
#include "preview/__type_traits/conjunction.h"
#include "preview/__type_traits/disjunction.h"
#include "preview/__type_traits/negation.h"
#include "preview/__utility/as_const.h"
#include "preview/__utility/integer_sequence.h"

// TODO: Integrate contract

#ifdef PREVIEW_DISABLE_ASSERT
#define PREVIEW_MDSPAN_DEBUG_ASSERT(expr)
#else
#include <cassert>
#define PREVIEW_MDSPAN_DEBUG_ASSERT(expr) assert(expr)
#endif

namespace preview {
namespace detail {

// ---------- helpers ----------

template<typename Seq1, typename Seq2>
struct extents_same_dynamic_positions;

template<std::size_t... I, std::size_t... J>
struct extents_same_dynamic_positions<std::index_sequence<I...>, std::index_sequence<J...>>
    : conjunction<bool_constant<(I == J) && (I == dynamic_extent)>...> {};

template<typename Seq, typename Out = std::index_sequence<0>, std::size_t DRank = 0>
struct extents_dynamic_index;

template<std::size_t... DExtents, std::size_t DRank>
struct extents_dynamic_index<std::index_sequence<>, std::index_sequence<DExtents...>, DRank> {
  static constexpr std::size_t value[sizeof...(DExtents)] = {DExtents...};
};

template<std::size_t Extent, std::size_t... Extents, std::size_t... DExtents, std::size_t DRank>
struct extents_dynamic_index<std::index_sequence<Extent, Extents...>, std::index_sequence<DExtents...>, DRank>
    : std::conditional_t<
        (Extent == dynamic_extent),
        extents_dynamic_index<std::index_sequence<Extents...>, std::index_sequence<DExtents..., DRank + 1>, DRank + 1>,
        extents_dynamic_index<std::index_sequence<Extents...>, std::index_sequence<DExtents..., DRank>, DRank>
    > {};

template<typename Seq, typename Out = std::index_sequence<>, std::size_t Index = 0>
struct extents_dynamic_index_inv;

template<std::size_t... IndexInv, std::size_t Index>
struct extents_dynamic_index_inv<std::index_sequence<>, std::index_sequence<IndexInv...>, Index> {
  static constexpr std::size_t value[sizeof...(IndexInv)] = {IndexInv...};
};

template<std::size_t Extent, std::size_t... Extents, std::size_t... IndexInv, std::size_t Index>
struct extents_dynamic_index_inv<std::index_sequence<Extent, Extents...>, std::index_sequence<IndexInv...>, Index>
    : std::conditional_t<
        (Extent == dynamic_extent),
        extents_dynamic_index_inv<std::index_sequence<Extents...>, std::index_sequence<IndexInv..., Index>, Index + 1>,
        extents_dynamic_index_inv<std::index_sequence<Extents...>, std::index_sequence<IndexInv...>, Index + 1>
    > {};


// ---------- extents_storage ----------

template<typename IndexType, std::size_t RankDynamic /* > 0 */, std::size_t... Extents>
struct extents_storage {
  using rank_type = std::size_t;

  static constexpr rank_type   kRank           = sizeof...(Extents);
  static constexpr std::size_t kExtents[kRank] = {Extents...};

  constexpr extents_storage() noexcept = default;

  template<typename OtherIndexType, std::size_t OtherRankDynamic, std::size_t... OtherExtents, std::enable_if_t<
      extents_same_dynamic_positions<std::index_sequence<Extents...>, std::index_sequence<OtherExtents...>>
  ::value, int> = 0>
  constexpr extents_storage(const extents_storage<OtherIndexType, OtherRankDynamic, OtherExtents...>& other) noexcept
      : dynamic_extents_(other.dynamic_extents_) {}

  template<typename OtherIndexType, std::size_t OtherRankDynamic, std::size_t... OtherExtents, std::size_t... I>
  constexpr explicit extents_storage(const extents_storage<OtherIndexType, OtherRankDynamic, OtherExtents...>& other, std::index_sequence<I...>) noexcept
      : dynamic_extents_{other.extent(dynamic_index_inv(I))...} {}

  template<typename OtherIndexType, std::size_t OtherRankDynamic, std::size_t... OtherExtents, std::enable_if_t<
      negation<extents_same_dynamic_positions<std::index_sequence<Extents...>, std::index_sequence<OtherExtents...>>>
  ::value, int> = 0>
  constexpr extents_storage(const extents_storage<OtherIndexType, OtherRankDynamic, OtherExtents...>& other) noexcept
      : extents_storage(other, std::make_index_sequence<RankDynamic>{}) {}

  template<typename SpanOrArray, std::size_t... d>
  constexpr explicit extents_storage(SpanOrArray&& exts, std::index_sequence<d...>, std::true_type /* N == rank_dynamic() */)
      : dynamic_extents_{preview::as_const(exts[d])...} {}

  template<typename SpanOrArray, std::size_t... d>
  constexpr explicit extents_storage(SpanOrArray&& exts, std::index_sequence<d...>, std::false_type /* N == rank_dynamic() */)
      : dynamic_extents_{preview::as_const(exts[dynamic_index_inv(d)])...} {}

  template<typename OtherIndexType, std::size_t N>
  constexpr extents_storage(span<OtherIndexType, N> exts)
      : extents_storage(exts, std::make_index_sequence<RankDynamic>{}, bool_constant<N == RankDynamic>{}){}

  template<typename OtherIndexType, std::size_t N, std::size_t... I>
  constexpr extents_storage(const std::array<OtherIndexType, N>& exts)
      : extents_storage(exts, std::make_index_sequence<RankDynamic>{}, bool_constant<N == RankDynamic>{}){}

  constexpr IndexType extent(rank_type i) const noexcept {
    return kExtents[i] == dynamic_extent ? dynamic_extents_[dynamic_index(i)] : kExtents[i];
  }
  static constexpr rank_type dynamic_index(rank_type i) noexcept {
    return dynamic_index_t::value[i];
  }
  static constexpr rank_type dynamic_index_inv(rank_type i) noexcept {
    return dynamic_index_inv_t::value[i];
  }

 private:
  using extents_sequence    = std::index_sequence<Extents...>;
  using dynamic_index_t     = extents_dynamic_index<extents_sequence>;
  using dynamic_index_inv_t = extents_dynamic_index_inv<extents_sequence>;

  template<typename Other>
  constexpr bool my_static_extent_is_equal_to_other_extent(const Other& other) const noexcept {
    for (rank_type r = 0; r < kRank; ++r) {
      if (kExtents[r] != dynamic_extent && kExtents[r] != other.extent(r))
        return false;
    }
    return true;
  }

  std::array<IndexType, RankDynamic> dynamic_extents_{};
};

template<typename IndexType, std::size_t... Extents>
struct extents_storage<IndexType, 0, Extents...> {
  using rank_type = std::size_t;

  static constexpr rank_type   kRank           = sizeof...(Extents);
  static constexpr std::size_t kExtents[kRank] = {Extents...};

  constexpr extents_storage() noexcept = default;

  template<typename OtherIndexType, std::size_t RankDynamic, std::size_t... OtherExtents, std::enable_if_t<RankDynamic != 0, int> = 0>
  constexpr extents_storage(const extents_storage<OtherIndexType, RankDynamic, OtherExtents...>& other) noexcept {
    // Preconditions
    PREVIEW_MDSPAN_DEBUG_ASSERT(my_static_extent_is_equal_to_other_extent(other));
  }

  template<typename OtherIndexType, std::size_t N>
  constexpr extents_storage(PREVIEW_MAYBE_UNUSED span<OtherIndexType, N> exts) {
    // Preconditions
    PREVIEW_MDSPAN_DEBUG_ASSERT(my_static_extent_is_equal_to_other_subscript(exts));
    PREVIEW_CXX14_SUPPRESS_UNUSED(exts);
  }

  template<typename OtherIndexType, std::size_t N>
  constexpr extents_storage(PREVIEW_MAYBE_UNUSED const std::array<OtherIndexType, N>& exts) {
    // Preconditions
    PREVIEW_MDSPAN_DEBUG_ASSERT(my_static_extent_is_equal_to_other_subscript(exts));
    PREVIEW_CXX14_SUPPRESS_UNUSED(exts);
  }

  constexpr IndexType extent(rank_type i) const noexcept {
    return kExtents[i];
  }
  static constexpr rank_type dynamic_index(rank_type) noexcept {
    return 0;
  }

 private:
  template<typename Other>
  constexpr bool my_static_extent_is_equal_to_other_extent(const Other& other) const noexcept {
    for (rank_type r = 0; r < kRank; ++r) {
      if (kExtents[r] != dynamic_extent && kExtents[r] != other.extent(r))
        return false;
    }
    return true;
  }

  template<typename Other>
  constexpr bool my_static_extent_is_equal_to_other_subscript(const Other& other) const noexcept {
    for (rank_type r = 0; r < kRank; ++r) {
      if (kExtents[r] != dynamic_extent && kExtents[r] != other[r])
        return false;
    }
    return true;
  }
};

} // namespace detail

template<typename IndexType, std::size_t... Extents>
class extents
    : protected detail::extents_storage<
        IndexType,
        integer_sequence_count<std::index_sequence<Extents...>, dynamic_extent>::value,
        Extents...>
{
  using storage_base = detail::extents_storage<
      IndexType,
      integer_sequence_count<std::index_sequence<Extents...>, dynamic_extent>::value,
      Extents...>;

  template<typename OtherIndexType, std::size_t... OtherExtents>
  friend class extents;

 public:
  using index_type = IndexType;
  using size_type = std::make_unsigned_t<index_type>;
  using rank_type = std::size_t;

  static constexpr rank_type rank() noexcept { return storage_base::kRank; }
  static constexpr rank_type rank_dynamic() noexcept { return storage_base::dynamic_index(rank()); }
  static constexpr std::size_t static_extent(rank_type r) noexcept {
    return storage_base::kExtents[r];
  }
  using storage_base::extent;

  constexpr extents() noexcept = default;

  template<typename OtherIndexType, std::size_t... OtherExtents, std::enable_if_t<conjunction<
      bool_constant<sizeof...(OtherExtents) == rank()>,
      bool_constant<(OtherExtents == dynamic_extent || Extents == dynamic_extent || OtherExtents == Extents)>...,
      // explicit(true)
      disjunction<
          bool_constant<(Extents != dynamic_extent) && (OtherExtents == dynamic_extent)>...,
          bool_constant<(std::numeric_limits<index_type>::max() < std::numeric_limits<OtherIndexType>::max())>
      >
  >::value, int> = 0>
  constexpr explicit extents(const extents<OtherIndexType, OtherExtents...>& other) noexcept
      : storage_base(other) {}

  template<typename OtherIndexType, std::size_t... OtherExtents, std::enable_if_t<conjunction<
      bool_constant<sizeof...(OtherExtents) == rank()>,
      bool_constant<(OtherExtents == dynamic_extent || Extents == dynamic_extent || OtherExtents == Extents)>...,
      // explicit(false)
      negation<disjunction<
          bool_constant<(Extents != dynamic_extent) && (OtherExtents == dynamic_extent)>...,
          bool_constant<(std::numeric_limits<index_type>::max() < std::numeric_limits<OtherIndexType>::max())>
      >>
  >::value, int> = 0>
  constexpr extents(const extents<OtherIndexType, OtherExtents...>& other) noexcept
      : storage_base(other) {}

  template<typename... OtherIndexType, std::enable_if_t<conjunction<
      std::is_convertible<OtherIndexType, index_type>...,
      std::is_nothrow_constructible<index_type, OtherIndexType>...,
      bool_constant<(sizeof...(OtherIndexType) == rank_dynamic()) || (sizeof...(OtherIndexType) == rank())>
  >::value, int> = 0>
  constexpr explicit extents(OtherIndexType... exts) noexcept
      : storage_base(std::array<index_type, sizeof...(OtherIndexType)>{static_cast<index_type>(std::move(exts))...}) {}

  // 10     Preconditions:
  // (10.1) If N != rank_dynamic() is true, exts[r] equals Er for each r for which Er is a static extent, and
  // (10.2) either
  // (10.2.1) N is zero, or
  // (10.2.2) exts[r] is representable as a nonnegative value of type index_type for every rank index r.

  template<typename OtherIndexType, std::size_t N, std::enable_if_t<conjunction<
      std::is_convertible<const OtherIndexType&, index_type>,
      std::is_nothrow_constructible<index_type, const OtherIndexType&>,
      bool_constant<(N == rank_dynamic() || N == rank())>,
      // explicit(true)
      bool_constant<(N != rank_dynamic())>
  >::value, int> = 0>
  constexpr explicit extents(span<OtherIndexType, N> exts) noexcept
      : storage_base(exts) {}

  template<typename OtherIndexType, std::size_t N, std::enable_if_t<conjunction<
      std::is_convertible<const OtherIndexType&, index_type>,
      std::is_nothrow_constructible<index_type, const OtherIndexType&>,
      bool_constant<(N == rank_dynamic() || N == rank())>,
      // explicit(false)
      bool_constant<(N == rank_dynamic())>
  >::value, int> = 0>
  constexpr extents(span<OtherIndexType, N> exts) noexcept
      : storage_base(exts) {}

  template<typename OtherIndexType, std::size_t N, std::enable_if_t<conjunction<
      std::is_convertible<const OtherIndexType&, index_type>,
      std::is_nothrow_constructible<index_type, const OtherIndexType&>,
      bool_constant<(N == rank_dynamic() || N == rank())>,
      // explicit(true)
      bool_constant<(N != rank_dynamic())>
  >::value, int> = 0>
  constexpr explicit extents(const std::array<OtherIndexType, N>& exts) noexcept
      : storage_base(exts) {}

  template<typename OtherIndexType, std::size_t N, std::enable_if_t<conjunction<
      std::is_convertible<const OtherIndexType&, index_type>,
      std::is_nothrow_constructible<index_type, const OtherIndexType&>,
      bool_constant<(N == rank_dynamic() || N == rank())>,
      // explicit(false)
      bool_constant<(N == rank_dynamic())>
  >::value, int> = 0>
  constexpr extents(const std::array<OtherIndexType, N>& exts) noexcept
      : storage_base(exts) {}

  template<typename OtherIndexType, std::size_t... OtherExtents>
  friend constexpr bool operator==(const extents& x, const extents<OtherIndexType, OtherExtents...>& y) noexcept {
    using x_type = extents;
    using y_type = extents<OtherIndexType, OtherExtents...>;

    if (x_type::rank() != y_type::rank())
      return false;
    for (rank_type r = 0; r < x_type::rank(); ++r) {
      if (x.extent(r) != y.extent(r))
        return false;
    }
    return true;
  }

  template<typename OtherIndexType, std::size_t... OtherExtents>
  friend constexpr bool operator!=(const extents& x, const extents<OtherIndexType, OtherExtents...>& y) noexcept {
    return !(x == y);
  }
};

namespace detail {

template<typename IndexType, std::size_t Rank, typename Helper = std::make_index_sequence<Rank>>
struct dextents_helper;
template<typename IndexType, std::size_t Rank, std::size_t... I>
struct dextents_helper<IndexType, Rank, std::index_sequence<I...>> {
  using type = extents<IndexType, ((void)I, dynamic_extent)...>;
};

} // namespace detail

template<typename IndexType, std::size_t Rank>
using dextents = typename detail::dextents_helper<IndexType, Rank>::type;

template<std::size_t Rank, class IndexType = std::size_t>
using dims = dextents<IndexType, Rank>;

} // namespace preview

#undef PREVIEW_MDSPAN_DEBUG_ASSERT

#endif // PREVIEW_MDSPAN_EXTENTS_H_
