//
// Created by yonggyulee on 2024. 9. 27.
//

#ifndef PREVIEW_MDSPAN_LAYOUT_H_
#define PREVIEW_MDSPAN_LAYOUT_H_

namespace preview {

struct layout_left {
  template<typename Extents>
  struct mapping {};
};

struct layout_right {
  template<typename Extents>
  struct mapping {};
};

struct layout_stride {
  template<typename Extents>
  struct mapping {};
};

} // namespace preview

#endif // PREVIEW_MDSPAN_LAYOUT_H_
