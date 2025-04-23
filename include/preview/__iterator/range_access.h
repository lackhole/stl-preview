//
// Created by yonggyulee on 14/04/2025
//

#ifndef PREVIEW_ITERATOR_RANGE_ACCESS_H_
#define PREVIEW_ITERATOR_RANGE_ACCESS_H_

#include <iterator>

#include "preview/__iterator/data.h"
#include "preview/__iterator/empty.h"
#include "preview/__iterator/size.h"
#include "preview/__iterator/ssize.h"

namespace preview {

using std::begin;
using std::end;
using std::rbegin;
using std::rend;
using std::cbegin;
using std::cend;
using std::crbegin;
using std::crend;

} // namespace preview

#endif // PREVIEW_ITERATOR_RANGE_ACCESS_H_
