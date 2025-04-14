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
template<typename OtherMapping>
constexpr layout_left::mapping<Extents>::mapping(detail::layout_mapping_ctor_tag, const OtherMapping& m) noexcept
    : extents_base(m.extents()) {}


// ---------- layout_right::mapping ----------

template<typename Extents>
template<typename OtherMapping>
constexpr layout_right::mapping<Extents>::mapping(detail::layout_mapping_ctor_tag, const OtherMapping& m) noexcept
    : extents_(m.extents()) {}

} // namespace preview

#endif // PREVIEW_MDSPAN_LAYOUT_BACKWARD_DECLARE_H_
