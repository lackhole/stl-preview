//
// Created by yonggyulee on 2023/09/29.
//

#ifndef PREVIEW_TYPE_TRAITS_TYPE_IDENTITY_H_
#define PREVIEW_TYPE_TRAITS_TYPE_IDENTITY_H_

namespace preview {

template<typename T>
struct type_identity {
  using type = T;
};

template<typename T>
using type_identity_t = typename type_identity<T>::type;

} // namespace preview

#endif // PREVIEW_TYPE_TRAITS_TYPE_IDENTITY_H_
