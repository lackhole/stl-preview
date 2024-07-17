//
// Created by yonggyulee on 2024. 7. 17.
//

#ifndef PREVIEW_FORMAT_BASIC_FORMATTER_H_
#define PREVIEW_FORMAT_BASIC_FORMATTER_H_

#include <type_traits>

#include "preview/__concepts/assignable_from.h"
#include "preview/__concepts/copy_constructible.h"
#include "preview/__concepts/default_initializable.h"
#include "preview/__concepts/destructible.h"
#include "preview/__concepts/swappable.h"
#include "preview/__type_traits/conjunction.h"
#include "preview/__type_traits/void_t.h"

namespace preview {
namespace detail {


} // namespace detail

template<typename T>
struct BasicFormatter
    : conjunction<
        default_initializable<T>,
        copy_constructible<T>,
        assignable_from<T&, const T&>,
        destructible<T>,
        swappable<T>
    > {};

} // namespace preview

#endif // PREVIEW_FORMAT_BASIC_FORMATTER_H_
