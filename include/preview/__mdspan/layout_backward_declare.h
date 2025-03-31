//
// Created by yonggyulee on 2024. 10. 27.
//

#if !defined(PREVIEW_MDSPAN_LAYOUT_FORWARD_DECLARE_H_) \
    || !defined(PREVIEW_MDSPAN_LAYOUT_LEFT_H_)         \
    || !defined(PREVIEW_MDSPAN_LAYOUT_RIGHT_H_)

#error "This file must be included after above files"

#endif

#ifndef PREVIEW_MDSPAN_LAYOUT_BACKWARD_DECLARE_H_
#define PREVIEW_MDSPAN_LAYOUT_BACKWARD_DECLARE_H_

#include "preview/__mdspan/layout_left.h"
#include "preview/__mdspan/layout_right.h"
#include "preview/__mdspan/layout_stride.h"

namespace preview {

// ---------- out-of-line definitions ----------

// ---------- layout_left::mapping ----------

template<typename Extents>
template<typename OtherExtents, std::enable_if_t<conjunction<
    bool_constant<(Extents::rank() <= 1)>,
    std::is_constructible<Extents, OtherExtents>,
    /* explicit(true) */ negation<std::is_convertible<OtherExtents, Extents>>
>::value, int>>
constexpr layout_left::mapping<Extents>::mapping(const layout_right::mapping<OtherExtents>& other) noexcept
    : extents_base(other.extents()) {}

template<typename Extents>
template<typename OtherExtents, std::enable_if_t<conjunction<
    bool_constant<(Extents::rank() <= 1)>,
    std::is_constructible<Extents, OtherExtents>,
    /* explicit(false) */ std::is_convertible<OtherExtents, Extents>
>::value, int>>
constexpr layout_left::mapping<Extents>::mapping(const layout_right::mapping<OtherExtents>& other) noexcept
    : extents_base(other.extents()) {}

template<typename Extents>
template<typename OtherExtents, std::enable_if_t<conjunction<
    std::is_constructible<Extents, OtherExtents>,
    /* explicit(true) */ bool_constant<(Extents::rank() > 0)>
>::value, int>>
constexpr layout_left::mapping<Extents>::mapping(const layout_stride::mapping<OtherExtents>& other) noexcept
    : extents_base(other.extents()) {}

template<typename Extents>
template<typename OtherExtents, std::enable_if_t<conjunction<
    std::is_constructible<Extents, OtherExtents>,
    /* explicit(false) */ bool_constant<!(Extents::rank() > 0)>
>::value, int>>
constexpr layout_left::mapping<Extents>::mapping(const layout_stride::mapping<OtherExtents>& other) noexcept
    : extents_base(other.extents()) {}


// ---------- layout_right::mapping ----------

template<typename Extents>
template<typename OtherExtents, std::enable_if_t<conjunction<
    bool_constant<(Extents::rank() <= 1)>,
    std::is_constructible<Extents, OtherExtents>,
    /* explicit(true) */ negation<std::is_convertible<OtherExtents, Extents>>
>::value, int>>
constexpr layout_right::mapping<Extents>::mapping(const layout_left::mapping<OtherExtents>& other) noexcept
    : extents_(other.extents()) {}

template<typename Extents>
template<typename OtherExtents, std::enable_if_t<conjunction<
    bool_constant<(Extents::rank() <= 1)>,
    std::is_constructible<Extents, OtherExtents>,
    /* explicit(false) */ std::is_convertible<OtherExtents, Extents>
>::value, int>>
constexpr layout_right::mapping<Extents>::mapping(const layout_left::mapping<OtherExtents>& other) noexcept
    : extents_(other.extents()) {}

template<typename Extents>
template<typename OtherExtents, std::enable_if_t<conjunction<
    std::is_constructible<Extents, OtherExtents>,
    /* explicit(true) */ bool_constant<(Extents::rank() > 0)>
>::value, int>>
constexpr layout_right::mapping<Extents>::mapping(const layout_stride::mapping<OtherExtents>& other) noexcept
    : extents_(other.extents()) {}

template<typename Extents>
template<typename OtherExtents, std::enable_if_t<conjunction<
    std::is_constructible<Extents, OtherExtents>,
    /* explicit(false) */ bool_constant<!(Extents::rank() > 0)>
>::value, int>>
constexpr layout_right::mapping<Extents>::mapping(const layout_stride::mapping<OtherExtents>& other) noexcept
    : extents_(other.extents()) {}

} // namespace preview

#endif // PREVIEW_MDSPAN_LAYOUT_BACKWARD_DECLARE_H_
