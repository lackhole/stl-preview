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
#include "preview/__concepts/integer_like.h"
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

template<typename T, T... I>
struct static_array {
//  static constexpr std::size_t size = sizeof...(I);
//  static constexpr T value(std::size_t i) { return integer_sequence_get_v}
  static constexpr std::size_t size = sizeof...(I);
  static constexpr T value[size] = {I...};
};

template<typename T, T... I>
constexpr T static_array<T, I...>::value[size];

template<typename Seq1, typename Seq2>
struct extents_same_dynamic_positions;

template<std::size_t... I, std::size_t... J>
struct extents_same_dynamic_positions<std::index_sequence<I...>, std::index_sequence<J...>>
    : conjunction<bool_constant<(I == J) && (I == dynamic_extent)>...> {};

template<typename Seq, typename Out = std::index_sequence<0>, std::size_t DRank = 0>
struct extents_dynamic_index;

template<std::size_t... DExtents, std::size_t DRank>
struct extents_dynamic_index<std::index_sequence<>, std::index_sequence<DExtents...>, DRank>
    : static_array<std::size_t, DExtents...> {};

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
struct extents_dynamic_index_inv<std::index_sequence<>, std::index_sequence<IndexInv...>, Index>
    : static_array<std::size_t, IndexInv...> {};

template<std::size_t Extent, std::size_t... Extents, std::size_t... IndexInv, std::size_t Index>
struct extents_dynamic_index_inv<std::index_sequence<Extent, Extents...>, std::index_sequence<IndexInv...>, Index>
    : std::conditional_t<
        (Extent == dynamic_extent),
        extents_dynamic_index_inv<std::index_sequence<Extents...>, std::index_sequence<IndexInv..., Index>, Index + 1>,
        extents_dynamic_index_inv<std::index_sequence<Extents...>, std::index_sequence<IndexInv...>, Index + 1>
    > {};

template<typename Seq, typename Out = std::index_sequence<1>, std::size_t Prod = 1>
struct extents_fwd_prod_of_extents;

template<std::size_t... Prods, std::size_t Prod>
struct extents_fwd_prod_of_extents<std::index_sequence<>, std::index_sequence<Prods...>, Prod>
    : static_array<std::size_t, Prods...> {};

template<std::size_t Extent, std::size_t... Extents, std::size_t... Prods, std::size_t Prod>
struct extents_fwd_prod_of_extents<std::index_sequence<Extent, Extents...>, std::index_sequence<Prods...>, Prod>
    : extents_fwd_prod_of_extents<std::index_sequence<Extents...>, std::index_sequence<Prods..., Prod * Extent>, Prod * Extent> {};

template<typename Seq, typename Out = std::index_sequence<1>, std::size_t Prod = 1>
struct extents_rev_prod_of_extents_impl;

template<std::size_t E0, std::size_t... Prods, std::size_t Prod>
struct extents_rev_prod_of_extents_impl<std::index_sequence<E0>, std::index_sequence<Prods...>, Prod>
    : static_array<std::size_t, Prods...> {};

template<std::size_t Extent, std::size_t... Extents, std::size_t... Prods, std::size_t Prod>
struct extents_rev_prod_of_extents_impl<std::index_sequence<Extent, Extents...>, std::index_sequence<Prods...>, Prod>
    : extents_rev_prod_of_extents_impl<std::index_sequence<Extents...>, std::index_sequence<Prod * Extent, Prods...>, Prod * Extent> {};

template<typename Seq>
struct extents_rev_prod_of_extents : extents_rev_prod_of_extents_impl<integer_sequence_reverse<Seq>> {};

// ---------- extents_storage ----------

template<typename IndexType, std::size_t RankDynamic /* > 0 */, std::size_t... Extents>
class extents_storage {
  template<typename OtherIndexType, std::size_t OtherRankDynamic, std::size_t... OtherExtents>
  friend class extents_storage;

 public:
  using rank_type = std::size_t;

  static constexpr rank_type   kRank           = sizeof...(Extents);
  static constexpr std::size_t kExtents[kRank] = {Extents...};

  constexpr extents_storage() noexcept = default;

  template<typename OtherIndexType, std::size_t OtherRankDynamic, std::size_t... OtherExtents, std::enable_if_t<
      extents_same_dynamic_positions<std::index_sequence<Extents...>, std::index_sequence<OtherExtents...>>
  ::value, int> = 0>
  constexpr extents_storage(const extents_storage<OtherIndexType, OtherRankDynamic, OtherExtents...>& other) noexcept
      : extents_storage(other.dynamic_extents_, std::make_index_sequence<RankDynamic>{}, std::true_type{}) {}

  template<typename OtherIndexType, std::size_t OtherRankDynamic, std::size_t... OtherExtents, std::size_t... I>
  constexpr explicit extents_storage(const extents_storage<OtherIndexType, OtherRankDynamic, OtherExtents...>& other, std::index_sequence<I...>) noexcept
      : dynamic_extents_{static_cast<IndexType>(other.extent(dynamic_index_inv(I)))...} {}

  template<typename OtherIndexType, std::size_t OtherRankDynamic, std::size_t... OtherExtents, std::enable_if_t<
      negation<extents_same_dynamic_positions<std::index_sequence<Extents...>, std::index_sequence<OtherExtents...>>>
  ::value, int> = 0>
  constexpr extents_storage(const extents_storage<OtherIndexType, OtherRankDynamic, OtherExtents...>& other) noexcept
      : extents_storage(other, std::make_index_sequence<RankDynamic>{}) {}

  template<typename SpanOrArray, std::size_t... d>
  constexpr explicit extents_storage(SpanOrArray&& exts, std::index_sequence<d...>, std::true_type /* N == rank_dynamic() */) noexcept
      : dynamic_extents_{static_cast<IndexType>(preview::as_const(exts[d]))...} {}

  template<typename SpanOrArray, std::size_t... d>
  constexpr explicit extents_storage(SpanOrArray&& exts, std::index_sequence<d...>, std::false_type /* N == rank_dynamic() */) noexcept
      : dynamic_extents_{static_cast<IndexType>(preview::as_const(exts[dynamic_index_inv(d)]))...} {}

  template<typename OtherIndexType, std::size_t N>
  constexpr extents_storage(span<OtherIndexType, N> exts) noexcept
      : extents_storage(exts, std::make_index_sequence<RankDynamic>{}, bool_constant<N == RankDynamic>{}){}

  template<typename OtherIndexType, std::size_t N, std::size_t... I>
  constexpr extents_storage(const std::array<OtherIndexType, N>& exts) noexcept
      : extents_storage(exts, std::make_index_sequence<RankDynamic>{}, bool_constant<N == RankDynamic>{}){}

  constexpr IndexType extent(rank_type i) const noexcept {
    return kExtents[i] == dynamic_extent ? dynamic_extents_[dynamic_index(i)] : static_cast<IndexType>(kExtents[i]);
  }
  static constexpr rank_type dynamic_index(rank_type i) noexcept {
    return dynamic_index_t::value[i];
  }
  static constexpr rank_type dynamic_index_inv(rank_type i) noexcept {
    return dynamic_index_inv_t::value[i];
  }

  constexpr std::size_t fwd_prod_of_extents(rank_type i) const noexcept {
    std::size_t p = 1;
    for (rank_type r = 0; r < i; ++r) {
      p *= extent(r);
    }
    return p;
  }

  constexpr std::size_t rev_prod_of_extents(rank_type i) const noexcept {
    std::size_t p = 1;
    for (rank_type r = i + 1; r < kRank; ++r) {
      p *= extent(r);
    }
    return p;
  }

  constexpr IndexType extent_dynamic(rank_type i) const noexcept {
    return dynamic_extents_[i];
  }
 protected:
  static constexpr bool index_space_is_representable() noexcept { return true; }

 private:
  using extents_sequence    = std::index_sequence<Extents...>;
  using dynamic_index_t     = extents_dynamic_index<extents_sequence>;
  using dynamic_index_inv_t = extents_dynamic_index_inv<extents_sequence>;

  template<typename Other>
  constexpr bool my_static_extent_is_equal_to_other_extent(const Other& other) const noexcept {
    using namespace preview::rel_ops;
    for (rank_type r = 0; r < kRank; ++r) {
      if (kExtents[r] != dynamic_extent && kExtents[r] != static_cast<IndexType>(other.extent(r)))
        return false;
    }
    return true;
  }

  std::array<IndexType, RankDynamic> dynamic_extents_{};
};

template<typename IndexType, std::size_t RankDynamic, std::size_t... Extents>
constexpr std::size_t extents_storage<IndexType, RankDynamic, Extents...>::kExtents[kRank];

template<typename IndexType, std::size_t... Extents>
class extents_storage<IndexType, 0, Extents...> {
 public:
  using rank_type = std::size_t;

  static constexpr rank_type   kRank           = sizeof...(Extents);
  // TODO: Remove +1
  static constexpr std::size_t kExtents[kRank + 1] = {Extents..., std::size_t{}};

  constexpr extents_storage() noexcept = default;

  template<typename OtherIndexType, std::size_t RankDynamic, std::size_t... OtherExtents>
  constexpr extents_storage(const extents_storage<OtherIndexType, RankDynamic, OtherExtents...>& other) noexcept {
    // Preconditions
    PREVIEW_MDSPAN_DEBUG_ASSERT(my_static_extent_is_equal_to_other_extent(other));
  }

  template<typename OtherIndexType, std::size_t N>
  constexpr extents_storage(PREVIEW_MAYBE_UNUSED span<OtherIndexType, N> exts) noexcept {
    // Preconditions
    PREVIEW_MDSPAN_DEBUG_ASSERT(my_static_extent_is_equal_to_other_subscript<N>(exts));
    PREVIEW_CXX14_SUPPRESS_UNUSED(exts);
  }

  template<typename OtherIndexType, std::size_t N>
  constexpr extents_storage(PREVIEW_MAYBE_UNUSED const std::array<OtherIndexType, N>& exts) noexcept {
    // Preconditions
    PREVIEW_MDSPAN_DEBUG_ASSERT(my_static_extent_is_equal_to_other_subscript<N>(exts));
    PREVIEW_CXX14_SUPPRESS_UNUSED(exts);
  }

  constexpr IndexType extent(rank_type i) const noexcept {
    return static_cast<IndexType>(kExtents[i]);
  }
  static constexpr rank_type dynamic_index(rank_type) noexcept {
    return 0;
  }

  constexpr std::size_t fwd_prod_of_extents(rank_type i) const noexcept {
    return extents_fwd_prod_of_extents<std::index_sequence<Extents...>>::value[i];
  }

  constexpr std::size_t rev_prod_of_extents(rank_type i) const noexcept {
    return extents_rev_prod_of_extents<std::index_sequence<Extents...>>::value[i];
  }
 protected:
  static constexpr bool index_space_is_representable() noexcept {
    IndexType space = 1;
    for (rank_type r = 0; r < kRank; ++r) {
      if (kExtents[r] != 0)
        space *= static_cast<IndexType>(kExtents[r]);
    }
    for (rank_type r = 0; r < kRank; ++r) {
      if (kExtents[r] != 0)
        space /= static_cast<IndexType>(kExtents[r]);
    }
    return space == 1;
  }

 private:
  template<typename Other>
  constexpr bool my_static_extent_is_equal_to_other_extent(const Other& other) const noexcept {
    using namespace preview::rel_ops;
    for (rank_type r = 0; r < kRank; ++r) {
      if (kExtents[r] != dynamic_extent && kExtents[r] != static_cast<IndexType>(other.extent(r)))
        return false;
    }
    return true;
  }

  template<std::size_t N, typename Other>
  constexpr bool my_static_extent_is_equal_to_other_subscript(const Other& other) const noexcept {
    if (N != 0) {
      using namespace preview::rel_ops;
      for (rank_type r = 0; r < kRank; ++r) {
        if (kExtents[r] != dynamic_extent && kExtents[r] != static_cast<IndexType>(other[r]))
          return false;
      }
    }
    return true;
  }
};

template<typename IndexType, std::size_t... Extents>
constexpr std::size_t extents_storage<IndexType, 0, Extents...>::kExtents[kRank + 1];

struct extents_helper {
  template<typename Extents>
  static constexpr std::size_t fwd_prod_of_extents(const Extents& e, std::size_t r) noexcept {
    return e.fwd_prod_of_extents(r);
  }

  template<typename Extents>
  static constexpr std::size_t rev_prod_of_extents(const Extents& e, std::size_t r) noexcept {
    return e.rev_prod_of_extents(r);
  }

  template<typename Extents, typename OtherIndexType>
  static constexpr auto index_cast(OtherIndexType&& i) noexcept {
    return Extents::index_cast(std::forward<OtherIndexType>(i));
  }

  template<typename Extents>
  static constexpr auto extent_dynamic(const Extents& e, typename Extents::rank_type r) noexcept {
    return e.extent_dynamic(r);
  }

  template<typename Extent>
  static constexpr bool index_space_is_representable() noexcept {
    return Extent::index_space_is_representable();
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
  static_assert(integer_like<IndexType>::value, "IndexType must be signed or unsigned integral type");
  static_assert(conjunction_v<bool_constant<
                    (Extents == dynamic_extent) ||
                    (Extents <= std::numeric_limits<IndexType>::max())>...>,
      "Extents must be either equal to dynamic_extent, or is representable as a value of type IndexType");

  using storage_base = detail::extents_storage<
      IndexType,
      integer_sequence_count<std::index_sequence<Extents...>, dynamic_extent>::value,
      Extents...>;

  constexpr const storage_base& base() const noexcept { return *this; }

  template<typename OtherIndexType, std::size_t... OtherExtents>
  friend class extents;

  using storage_base::fwd_prod_of_extents;
  using storage_base::rev_prod_of_extents;
  using storage_base::index_space_is_representable;

  template<typename OtherIndexType, std::enable_if_t<conjunction_v<
      std::is_integral<remove_cvref_t<OtherIndexType>>,
      negation<std::is_same<remove_cvref_t<OtherIndexType>, bool>>
  >, int> = 0>
  static constexpr auto index_cast(OtherIndexType&& i) noexcept {
    return i;
  }
  template<typename OtherIndexType, std::enable_if_t<!conjunction_v<
      std::is_integral<remove_cvref_t<OtherIndexType>>,
      negation<std::is_same<remove_cvref_t<OtherIndexType>, bool>>
  >, int> = 0>
  static constexpr auto index_cast(OtherIndexType&& i) noexcept {
    return static_cast<index_type>(i);
  }

  friend struct detail::extents_helper;

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
      : storage_base(other.base()) {}

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
      : storage_base(other.base()) {}

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

#if PREVIEW_CXX_VERSION >= 17

template<typename... Integrals>
extents(Integrals...)
    -> extents<
        std::enable_if_t<conjunction_v<std::is_convertible<Integrals, std::size_t>...>, std::size_t>,
        detail::maybe_static_ext<Integrals>::value...
    >;

#endif

namespace detail {

template<typename T>
struct is_extents : std::false_type {};
template<typename IndexType, std::size_t... Args>
struct is_extents<extents<IndexType, Args...>> : std::true_type {};

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
