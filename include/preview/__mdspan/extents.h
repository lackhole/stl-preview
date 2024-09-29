//
// Created by yonggyulee on 2024. 9. 27.
//

#ifndef PREVIEW_MDSPAN_EXTENTS_H_
#define PREVIEW_MDSPAN_EXTENTS_H_

#include <array>
#include <cassert>
#include <cstddef>
#include <limits>
#include <type_traits>
#include <utility>

#include "preview/__span/dynamic_extent.h"
#include "preview/__type_traits/bool_constant.h"
#include "preview/__type_traits/conjunction.h"
#include "preview/__type_traits/disjunction.h"
#include "preview/__type_traits/negation.h"
#include "preview/__utility/integer_sequence.h"

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
  static constexpr std::size_t value[] = {DExtents...};
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
  static constexpr std::size_t value[] = {IndexInv...};
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

  constexpr extents_storage() noexcept = default;

  template<typename OtherIndexType, std::size_t OtherRankDynamic, std::size_t... OtherExtents, std::enable_if_t<
      extents_same_dynamic_positions<std::index_sequence<Extents...>, std::index_sequence<OtherExtents...>>
  ::value, int> = 0>
  constexpr extents_storage(const extents_storage<OtherIndexType, OtherRankDynamic, OtherExtents...>& other) noexcept
      : dynamic_extents_(other.dynamic_extents_) {}

  template<typename OtherIndexType, std::size_t OtherRankDynamic, std::size_t... OtherExtents, std::enable_if_t<
      negation<extents_same_dynamic_positions<std::index_sequence<Extents...>, std::index_sequence<OtherExtents...>>>
  ::value, int> = 0>
  constexpr extents_storage(const extents_storage<OtherIndexType, OtherRankDynamic, OtherExtents...>& other) noexcept {
    // Preconditions
    assert(my_static_extent_is_equal_to_other_extent(other));

    for (rank_type i = 0; i < RankDynamic; ++i) {
      dynamic_extents_[i] = other.extent(dynamic_index_inv(i));
    }
  }

  template<typename OtherIndexType, std::size_t... OtherExtents, std::enable_if_t<
      negation<extents_same_dynamic_positions<std::index_sequence<Extents...>, std::index_sequence<OtherExtents...>>>
  ::value, int> = 0>
  constexpr extents_storage(const extents_storage<OtherIndexType, 0, OtherExtents...>& other) noexcept {
    for (rank_type i = 0; i < RankDynamic; ++i) {
      dynamic_extents_[i] = other.extent(dynamic_index_inv(i));
    }
  }

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

  static constexpr rank_type   kRank           = sizeof...(Extents);
  static constexpr std::size_t kExtents[kRank] = {Extents...};

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

  constexpr extents_storage() noexcept = default;

  template<typename OtherIndexType, std::size_t RankDynamic, std::size_t... OtherExtents, std::enable_if_t<RankDynamic != 0, int> = 0>
  constexpr extents_storage(const extents_storage<OtherIndexType, RankDynamic, OtherExtents...>& other) noexcept {
    // Preconditions
    assert(my_static_extent_is_equal_to_other_extent(other));
  }

  constexpr IndexType extent(rank_type i) const noexcept {
    return kExtents[i];
  }
  static constexpr rank_type dynamic_index(rank_type) noexcept {
    return 0;
  }
  static constexpr rank_type dynamic_index_inv(rank_type) noexcept {
    assert(false);
    return 0;
  }

 private:
  static constexpr rank_type   kRank           = sizeof...(Extents);
  static constexpr std::size_t kExtents[] = {Extents...};

  template<typename Other>
  constexpr bool my_static_extent_is_equal_to_other_extent(const Other& other) const noexcept {
    for (rank_type r = 0; r < kRank; ++r) {
      if (kExtents[r] != dynamic_extent && kExtents[r] != other.extent(r))
        return false;
    }
    return true;
  }
};

} // namespace detail

template<typename IndexType, std::size_t... Extents>
class extents
    : public detail::extents_storage<
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

  static constexpr rank_type rank() noexcept { return sizeof...(Extents); }
  static constexpr rank_type rank_dynamic() noexcept { return storage_base::dynamic_index(rank()); }
//  static constexpr std::size_t static_extent(rank_type r) noexcept {
//
//  }
  constexpr index_type extent(rank_type i) const noexcept {
    return storage_base::extent(i);
  }

  constexpr extents() noexcept = default;

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

  template<typename OtherIndexType, std::size_t... OtherExtents>
  friend constexpr bool operator==(const extents& x, const extents<OtherIndexType, OtherExtents...>& y) noexcept {
    if (decltype(x)::rank() != decltype(y)::rank())
      return false;
    for (rank_type r = 0; r < decltype(x)::rank(); ++r) {
      if (x.extent(r) != y.extent(r))
        return false;
    }
    return true;
  }
};

//template<typename IndexType, std::size_t Rank>
//using dextents;
//
//template<std::size_t Rank, class IndexType = std::size_t>
//using dims = see below;

} // namespace preview

#endif // PREVIEW_MDSPAN_EXTENTS_H_
