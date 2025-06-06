//
// Created by yonggyulee on 2024. 10. 27.
//

#ifndef PREVIEW_MDSPAN_LAYOUT_LEFT_H_
#define PREVIEW_MDSPAN_LAYOUT_LEFT_H_

#include <type_traits>
#include <utility>

#include "preview/__array/to_array.h"
#include "preview/__mdspan/layout_forward_declare.h"
#include "preview/__mdspan/extents.h"
#include "preview/__type_traits/conjunction.h"
#include "preview/__type_traits/negation.h"
#include "preview/__utility/cxx20_rel_ops.h"

namespace preview {

template<typename Extents>
class layout_left::mapping : private Extents {
  static_assert(detail::is_extents<Extents>::value, "Extents must be a specialization of extents");
  static_assert(detail::extents_helper::index_space_is_representable<Extents>(),
      "index space Extents() must be representable as a value of type typename Extents::index_type");

  using extents_base = Extents;

  template<typename OtherMapping>
  constexpr mapping(detail::layout_mapping_ctor_tag, const OtherMapping& m) noexcept;

 public:
  using extents_type = Extents;
  using index_type   = typename extents_type::index_type;
  using size_type    = typename extents_type::size_type;
  using rank_type    = typename extents_type::rank_type;
  using layout_type  = layout_left;

  constexpr mapping() noexcept = default;
  constexpr mapping(const mapping&) noexcept = default;
  constexpr mapping(const extents_type& e) noexcept
      : extents_base(e) {}

  template<typename OtherExtents, std::enable_if_t<conjunction<
      std::is_constructible<extents_type, OtherExtents>,
      /* explicit(true) */ negation<std::is_convertible<OtherExtents, extents_type>>
  >::value, int> = 0>
  constexpr explicit mapping(const mapping<OtherExtents>& other) noexcept
      : extents_base(other.extents()) {}

  template<typename OtherExtents, std::enable_if_t<conjunction<
      std::is_constructible<extents_type, OtherExtents>,
      /* explicit(false) */ std::is_convertible<OtherExtents, extents_type>
  >::value, int> = 0>
  constexpr mapping(const mapping<OtherExtents>& other) noexcept
      : extents_base(other.extents()) {}

  template<typename OtherExtents, std::enable_if_t<conjunction<
      bool_constant<(extents_type::rank() <= 1)>,
      std::is_constructible<extents_type, OtherExtents>,
      /* explicit(true) */ negation<std::is_convertible<OtherExtents, extents_type>>
  >::value, int> = 0>
  constexpr explicit mapping(const layout_right::mapping<OtherExtents>& other) noexcept
      : mapping(detail::layout_mapping_ctor_tag{}, other) {}

  template<typename OtherExtents, std::enable_if_t<conjunction<
      bool_constant<(extents_type::rank() <= 1)>,
      std::is_constructible<extents_type, OtherExtents>,
      /* explicit(false) */ std::is_convertible<OtherExtents, extents_type>
  >::value, int> = 0>
  constexpr mapping(const layout_right::mapping<OtherExtents>& other) noexcept
      : mapping(detail::layout_mapping_ctor_tag{}, other) {}

  template<typename LayoutLeftPaddedMapping, std::enable_if_t<conjunction<
      detail::is_layout_left_padded_mapping_of<LayoutLeftPaddedMapping>,
      std::is_constructible<extents_type, typename LayoutLeftPaddedMapping::extents_type>,
      /* explicit(true) */ negation<std::is_convertible<typename LayoutLeftPaddedMapping::extents_type, extents_type>>
  >::value, int> = 0>
  constexpr explicit mapping(const LayoutLeftPaddedMapping& other) noexcept
      : extents_base(other.extents()) {}

  template<typename LayoutLeftPaddedMapping, std::enable_if_t<conjunction<
      detail::is_layout_left_padded_mapping_of<LayoutLeftPaddedMapping>,
      std::is_constructible<extents_type, typename LayoutLeftPaddedMapping::extents_type>,
      /* explicit(false) */ std::is_convertible<typename LayoutLeftPaddedMapping::extents_type, extents_type>
  >::value, int> = 0>
  constexpr mapping(const LayoutLeftPaddedMapping& other) noexcept
      : extents_base(other.extents()) {}


  // TODO: Investigate noexcept
  template<typename OtherExtents, std::enable_if_t<conjunction<
      std::is_constructible<extents_type, OtherExtents>,
      /* explicit(true) */ bool_constant<(extents_type::rank() > 0)>
  >::value, int> = 0>
  constexpr explicit mapping(const layout_stride::mapping<OtherExtents>& other) noexcept
      : mapping(detail::layout_mapping_ctor_tag{}, other) {}

  template<typename OtherExtents, std::enable_if_t<conjunction<
      std::is_constructible<extents_type, OtherExtents>,
      /* explicit(false) */ bool_constant<!(extents_type::rank() > 0)>
  >::value, int> = 0>
  constexpr mapping(const layout_stride::mapping<OtherExtents>& other) noexcept
      : mapping(detail::layout_mapping_ctor_tag{}, other) {}

  constexpr mapping& operator=(const mapping&) noexcept = default;

  // [mdspan.layout.left.obs], observers
  constexpr const extents_type& extents() const noexcept { return static_cast<const extents_type&>(*this); }

  constexpr index_type required_span_size() const noexcept {
    return static_cast<index_type>(detail::extents_helper::fwd_prod_of_extents(extents(), extents_type::rank()));
  }

#if PREVIEW_CXX_VERSION < 17
  template<bool B = (extents_type::rank() == 0), std::enable_if_t<B, int> = 0>
  constexpr index_type operator()() const noexcept {
    return 0;
  }

  template<typename... Indices, std::enable_if_t<conjunction<
      bool_constant<extents_type::rank() != 0>,
      bool_constant<(sizeof...(Indices) == extents_type::rank())>,
      std::is_constructible<Indices, index_type>...,
      std::is_nothrow_constructible<index_type, Indices>...
  >::value, int> = 0>
  constexpr index_type operator()(Indices... i) const noexcept {
    return call(preview::to_array({static_cast<index_type>(std::move(i))...}));
  }
#else
  template<typename... Indices, std::enable_if_t<conjunction<
      bool_constant<(sizeof...(Indices) == extents_type::rank())>,
      std::is_constructible<Indices, index_type>...,
      std::is_nothrow_constructible<index_type, Indices>...
  >::value, int> = 0>
  constexpr index_type operator()(Indices... i) const noexcept {
    return call(std::index_sequence_for<Indices...>{}, static_cast<index_type>(std::move(i))...);
  }
#endif

  static constexpr bool is_always_unique() noexcept { return true; }
  static constexpr bool is_always_exhaustive() noexcept { return true; }
  static constexpr bool is_always_strided() noexcept { return true; }

  static constexpr bool is_unique() noexcept { return true; }
  static constexpr bool is_exhaustive() noexcept { return true; }
  static constexpr bool is_strided() noexcept { return true; }

  template<typename Dummy = void, std::enable_if_t<std::is_void<Dummy>::value && (extents_type::rank() > 0), int> = 0>
  constexpr index_type stride(rank_type i) const noexcept {
    return static_cast<index_type>(detail::extents_helper::fwd_prod_of_extents(extents(), i));
  }

  template<typename OtherExtents, std::enable_if_t<(extents_type::rank() == OtherExtents::rank()), int> = 0>
  friend constexpr bool operator==(const mapping& x, const mapping<OtherExtents>& y) noexcept {
    PREVIEW_BEFORE_CXX20(using namespace preview::rel_ops);
    return x.extents() == y.extents();
  }

#if !PREVIEW_CONFORM_CXX20_STANDARD
  friend constexpr bool operator==(const mapping& x, const mapping& y) noexcept {
    return x.extents() == y.extents();
  }

  template<typename OtherExtents, std::enable_if_t<(extents_type::rank() == OtherExtents::rank()), int> = 0>
  friend constexpr bool operator!=(const mapping& x, const mapping<OtherExtents>& y) noexcept {
    return !(x == y);
  }

  friend constexpr bool operator!=(const mapping& x, const mapping& y) noexcept {
    return !(x == y);
  }
#endif

  // TODO
//  template<typename... SliceSpecifiers, std::enable_if_t<(sizeof...(SliceSpecifiers) == extents_type::rank()), int> = 0>
//  friend constexpr auto submdspan_mapping(const mapping& src, SliceSpecifiers... slices) {
//
//
//    using tag = preview::detail::conditional_tag<
//        bool_constant<(Extents::rank() == 0)>,
//        bool_constant<(SubExtents::rank() == 0)>
//    >;
//
//    return src.submdspan_mapping_impl(slices...);
//  }

 private:
#if PREVIEW_CXX_VERSION < 17
  template<std::size_t Rank>
  constexpr index_type call(const std::array<index_type, Rank>& indices) const noexcept {
    index_type result = 0;
    for (std::size_t r = 0; r < Rank; ++r) {
      result += indices[r] * stride(r);
    }
    return result;
  }
  template<typename I0>
  constexpr index_type call(const std::array<index_type, 1>& indices) const noexcept {
    return indices[0] * stride(0);
  }
  template<typename I0>
  constexpr index_type call(const std::array<index_type, 2>& indices) const noexcept {
    return indices[0] * stride(0) + indices[1] * stride(1);
  }
  template<typename I0>
  constexpr index_type call(const std::array<index_type, 3>& indices) const noexcept {
    return indices[0] * stride(0) + indices[1] * stride(1) + indices[2] * stride(2);
  }
  template<typename I0>
  constexpr index_type call(const std::array<index_type, 4>& indices) const noexcept {
    return indices[0] * stride(0) + indices[1] * stride(1) + indices[2] * stride(2) + indices[3] * stride(3);
  }
#else
  template<std::size_t... P, typename... Indices>
  constexpr index_type call(std::index_sequence<P...>, Indices... i) const noexcept {
    return ((i * stride(P)) + ... + 0);
  }
#endif // exposition only

  // [mdspan.sub.map], submdspan mapping specialization
//  template<typename... SliceSpecifiers>
//  constexpr auto submdspan_mapping_impl(SliceSpecifiers... slices) const {
//    // TODO
//    return 0;
//  }
};


} // namespace preview

#endif // PREVIEW_MDSPAN_LAYOUT_LEFT_H_
