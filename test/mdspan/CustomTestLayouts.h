// -*- C++ -*-
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//                        Kokkos v. 4.0
//       Copyright (2022) National Technology & Engineering
//               Solutions of Sandia, LLC (NTESS).
//
// Under the terms of Contract DE-NA0003525 with NTESS,
// the U.S. Government retains certain rights in this software.
//
//===---------------------------------------------------------------------===//

#ifndef TEST_STD_CONTAINERS_VIEWS_MDSPAN_CUSTOM_TEST_LAYOUTS_H
#define TEST_STD_CONTAINERS_VIEWS_MDSPAN_CUSTOM_TEST_LAYOUTS_H

#include <algorithm>
#include <array>
#include <cassert>
#include <cinttypes>
#include <cstddef>
#include <limits>
#include <type_traits>
#include <utility>

#include "preview/concepts.h"
#include "preview/mdspan.h"
#include "preview/span.h"

// Layout that wraps indices to test some idiosyncratic behavior
// - basically it is a layout_left where indices are first wrapped i.e. i%Wrap
// - only accepts integers as indices
// - is_always_strided and is_always_unique are false
// - is_strided and is_unique are true if all extents are smaller than Wrap
// - not default constructible
// - not extents constructible
// - not trivially copyable
// - does not check dynamic to static extent conversion in converting ctor
// - check via side-effects that mdspan::swap calls mappings swap via ADL

struct not_extents_constructible_tag {};

template <size_t Wrap>
class layout_wrapping_integral {
public:
  template <class Extents>
  class mapping;
};

template <size_t WrapArg>
template <class Extents>
class layout_wrapping_integral<WrapArg>::mapping {
  static constexpr typename Extents::index_type Wrap = static_cast<typename Extents::index_type>(WrapArg);

public:
  using extents_type = Extents;
  using index_type   = typename extents_type::index_type;
  using size_type    = typename extents_type::size_type;
  using rank_type    = typename extents_type::rank_type;
  using layout_type  = layout_wrapping_integral<Wrap>;

private:
  static constexpr bool required_span_size_is_representable(const extents_type& ext) {
    if (extents_type::rank() == 0)
      return true;

    index_type prod = ext.extent(0);
//    for (rank_type r = 1; r < extents_type::rank(); r++) {
//      bool overflowed = __builtin_mul_overflow(prod, std::min(ext.extent(r), Wrap), &prod);
//      if (overflowed)
//        return false;
//    }
    return true;
  }

public:
  constexpr mapping() noexcept = delete;

  constexpr mapping(const mapping& other) noexcept : extents_(other.extents()) {}

  template<bool B = (Wrap == 8), std::enable_if_t<B, int> = 0>
  constexpr mapping(extents_type&& ext) noexcept
      : extents_(ext) {}

  constexpr mapping(const extents_type& ext, not_extents_constructible_tag) noexcept : extents_(ext) {}

  template <class OtherExtents, std::enable_if_t<
      std::is_constructible<extents_type, OtherExtents>::value && (Wrap != 8) &&
      /* explicit(true) */ !std::is_convertible<OtherExtents, extents_type>::value
  , int> = 0>
  constexpr explicit mapping(const mapping<OtherExtents>& other) noexcept {
    std::array<index_type, extents_type::rank_dynamic()> dyn_extents;
    rank_type count = 0;
    for (rank_type r = 0; r < extents_type::rank(); r++) {
      if (extents_type::static_extent(r) == preview::dynamic_extent) {
        dyn_extents[count++] = other.extents().extent(r);
      }
    }
    extents_ = extents_type(dyn_extents);
  }

  template <class OtherExtents, std::enable_if_t<
      std::is_constructible<extents_type, OtherExtents>::value && (Wrap != 8) &&
      /* explicit(false) */ std::is_convertible<OtherExtents, extents_type>::value
  , int> = 0>
  constexpr mapping(const mapping<OtherExtents>& other) noexcept {
    std::array<index_type, extents_type::rank_dynamic()> dyn_extents;
    rank_type count = 0;
    for (rank_type r = 0; r < extents_type::rank(); r++) {
      if (extents_type::static_extent(r) == preview::dynamic_extent) {
        dyn_extents[count++] = other.extents().extent(r);
      }
    }
    extents_ = extents_type(dyn_extents);
  }

  template <class OtherExtents, std::enable_if_t<
      std::is_constructible<extents_type, OtherExtents>::value && (Wrap == 8) &&
      /* explicit(true) */ !std::is_convertible<OtherExtents, extents_type>::value
  , int> = 0>
  constexpr explicit mapping(mapping<OtherExtents>&& other) noexcept {
    std::array<index_type, extents_type::rank_dynamic()> dyn_extents;
    rank_type count = 0;
    for (rank_type r = 0; r < extents_type::rank(); r++) {
      if (extents_type::static_extent(r) == preview::dynamic_extent) {
        dyn_extents[count++] = other.extents().extent(r);
      }
    }
    extents_ = extents_type(dyn_extents);
  }

  template <class OtherExtents, std::enable_if_t<
      std::is_constructible<extents_type, OtherExtents>::value && (Wrap == 8) &&
      /* explicit(false) */ std::is_convertible<OtherExtents, extents_type>::value
  , int> = 0>
  constexpr mapping(mapping<OtherExtents>&& other) noexcept {
    std::array<index_type, extents_type::rank_dynamic()> dyn_extents;
    rank_type count = 0;
    for (rank_type r = 0; r < extents_type::rank(); r++) {
      if (extents_type::static_extent(r) == preview::dynamic_extent) {
        dyn_extents[count++] = other.extents().extent(r);
      }
    }
    extents_ = extents_type(dyn_extents);
  }

  constexpr mapping& operator=(const mapping& other) noexcept {
    extents_ = other.extents_;
    return *this;
  };

  constexpr const extents_type& extents() const noexcept { return extents_; }

  constexpr index_type required_span_size() const noexcept {
    index_type size = 1;
    for (size_t r = 0; r < extents_type::rank(); r++)
      size *= extents_.extent(r) < Wrap ? extents_.extent(r) : Wrap;
    return size;
  }

  template <typename... Indices, std::enable_if_t<preview::conjunction_v<
      preview::integral<Indices>...,
      preview::bool_constant<(sizeof...(Indices) == extents_type::rank())>,
      std::is_convertible<Indices, index_type>...,
      std::is_nothrow_constructible<index_type, Indices>...
  >, int> = 0>
  constexpr index_type operator()(Indices... idx) const noexcept {
    std::array<index_type, extents_type::rank()> idx_a{static_cast<index_type>(static_cast<index_type>(idx) % Wrap)...};
    index_type res = 0;
    for (std::size_t pos = 0; pos < sizeof...(Indices); ++pos) {
      res = idx_a[extents_type::rank() - 1 - pos] +
            (extents_.extent(extents_type::rank() - 1 - pos) < Wrap ? extents_.extent(extents_type::rank() - 1 - pos)
                                                                    : Wrap) *
                res;
    }
    return res;
  }

  static constexpr bool is_always_unique() noexcept { return false; }
  static constexpr bool is_always_exhaustive() noexcept { return true; }
  static constexpr bool is_always_strided() noexcept { return false; }

  constexpr bool is_unique() const noexcept {
    for (rank_type r = 0; r < extents_type::rank(); r++) {
      if (extents_.extent(r) > Wrap)
        return false;
    }
    return true;
  }
  static constexpr bool is_exhaustive() noexcept { return true; }
  constexpr bool is_strided() const noexcept {
    for (rank_type r = 0; r < extents_type::rank(); r++) {
      if (extents_.extent(r) > Wrap)
        return false;
    }
    return true;
  }

  template<bool B = (extents_type::rank() > 0), std::enable_if_t<B, int> = 0>
  constexpr index_type stride(rank_type r) const noexcept
  {
    index_type s = 1;
    for (rank_type i = extents_type::rank() - 1; i > r; i--)
      s *= extents_.extent(i);
    return s;
  }

  template <class OtherExtents, std::enable_if_t<(OtherExtents::rank() == extents_type::rank()), int> = 0>
  friend constexpr bool operator==(const mapping& lhs, const mapping<OtherExtents>& rhs) noexcept {
    return lhs.extents() == rhs.extents();
  }

  friend constexpr void swap(mapping& x, mapping& y) noexcept {
    swap(x.extents_, y.extents_);
//    if (!std::is_constant_evaluated()) {
      swap_counter()++;
//    }
  }

  static int& swap_counter() {
    static int value = 0;
    return value;
  }

private:
  extents_type extents_{};
};

template <class Extents>
constexpr auto construct_mapping(preview::layout_left, Extents exts) {
  return preview::layout_left::mapping<Extents>(exts);
}

template <class Extents>
constexpr auto construct_mapping(preview::layout_right, Extents exts) {
  return preview::layout_right::mapping<Extents>(exts);
}

template <size_t Wraps, class Extents>
constexpr auto construct_mapping(layout_wrapping_integral<Wraps>, Extents exts) {
  return typename layout_wrapping_integral<Wraps>::template mapping<Extents>(exts, not_extents_constructible_tag{});
}

// This layout does not check convertibility of extents for its conversion ctor
// Allows triggering mdspan's ctor static assertion on convertibility of extents
// It also allows for negative strides and offsets via runtime arguments
class always_convertible_layout {
public:
  template <class Extents>
  class mapping;
};

template <class Extents>
class always_convertible_layout::mapping {
public:
  using extents_type = Extents;
  using index_type   = typename extents_type::index_type;
  using size_type    = typename extents_type::size_type;
  using rank_type    = typename extents_type::rank_type;
  using layout_type  = always_convertible_layout;

private:
  static constexpr bool required_span_size_is_representable(const extents_type& ext) {
    if (extents_type::rank() == 0)
      return true;

//    index_type prod = ext.extent(0);
//    for (rank_type r = 1; r < extents_type::rank(); r++) {
//      bool overflowed = __builtin_mul_overflow(prod, ext.extent(r), &prod);
//      if (overflowed)
//        return false;
//    }
    return true;
  }

public:
  constexpr mapping() noexcept = delete;
  constexpr mapping(const mapping& other) noexcept
      : extents_(other.extents_), offset_(other.offset_), scaling_(other.scaling_) {}
  constexpr mapping(const extents_type& ext, index_type offset = 0, index_type scaling = 1) noexcept
      : extents_(ext), offset_(offset), scaling_(scaling) {}

  template <class OtherExtents>
  constexpr mapping(const mapping<OtherExtents>& other) noexcept {
    if PREVIEW_CONSTEXPR_AFTER_CXX17 (extents_type::rank() == OtherExtents::rank()) {
      std::array<index_type, extents_type::rank_dynamic()> dyn_extents;
      rank_type count = 0;
      for (rank_type r = 0; r < extents_type::rank(); r++) {
        if (extents_type::static_extent(r) == preview::dynamic_extent) {
          dyn_extents[count++] = other.extents().extent(r);
        }
      }
      extents_ = extents_type(dyn_extents);
    } else {
      extents_ = extents_type();
    }
    offset_  = other.offset_;
    scaling_ = other.scaling_;
  }

  constexpr mapping& operator=(const mapping& other) noexcept {
    extents_ = other.extents_;
    offset_  = other.offset_;
    scaling_ = other.scaling_;
    return *this;
  };

  constexpr const extents_type& extents() const noexcept { return extents_; }

  constexpr index_type required_span_size() const noexcept {
    index_type size = 1;
    for (size_t r = 0; r < extents_type::rank(); r++)
      size *= extents_.extent(r);
    return std::max(size * scaling_ + offset_, offset_);
  }

  template <typename... Indices, std::enable_if_t<preview::conjunction_v<
      preview::integral<Indices>...,
      preview::bool_constant<(sizeof...(Indices) == extents_type::rank())>,
      std::is_convertible<Indices, index_type>...,
      std::is_nothrow_constructible<index_type, Indices>...
  >, int> = 0>
  constexpr index_type operator()(Indices... idx) const noexcept {
    std::array<index_type, extents_type::rank()> idx_a{static_cast<index_type>(static_cast<index_type>(idx))...};
    index_type res = 0;
    for (std::size_t pos = 0; pos < sizeof...(Indices); ++pos) {
      res = idx_a[extents_type::rank() - 1 - pos] + extents_.extent(extents_type::rank() - 1 - pos) * res;
    }

    return offset_ + scaling_ * res;
  }

  static constexpr bool is_always_unique() noexcept { return true; }
  static constexpr bool is_always_exhaustive() noexcept { return true; }
  static constexpr bool is_always_strided() noexcept { return true; }

  static constexpr bool is_unique() noexcept { return true; }
  static constexpr bool is_exhaustive() noexcept { return true; }
  static constexpr bool is_strided() noexcept { return true; }

  template<bool B = (extents_type::rank() > 0), std::enable_if_t<B, int> = 0>
  constexpr index_type stride(rank_type r) const noexcept
  {
    index_type s = 1;
    for (rank_type i = 0; i < r; i++)
      s *= extents_.extent(i);
    return s * scaling_;
  }

  template <class OtherExtents, std::enable_if_t<OtherExtents::rank() == extents_type::rank(), int> = 0>
  friend constexpr bool operator==(const mapping& lhs, const mapping<OtherExtents>& rhs) noexcept {
    return lhs.extents() == rhs.extents() && lhs.offset_ == rhs.offset && lhs.scaling_ == rhs.scaling_;
  }

  friend constexpr void swap(mapping& x, mapping& y) noexcept {
    swap(x.extents_, y.extents_);
//    if (!std::is_constant_evaluated()) {
      swap_counter()++;
//    }
  }

  static int& swap_counter() {
    static int value = 0;
    return value;
  }

private:
  template <class>
  friend class mapping;

  extents_type extents_{};
  index_type offset_{};
  index_type scaling_{};
};
#endif // TEST_STD_CONTAINERS_VIEWS_MDSPAN_CUSTOM_TEST_LAYOUTS_H
