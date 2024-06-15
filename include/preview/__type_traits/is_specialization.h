# /*
#  * Created by YongGyu Lee on 2020/12/08.
#  */
#
# ifndef PREVIEW_TYPE_TRAITS_IS_SPECIALIZATION_H_
# define PREVIEW_TYPE_TRAITS_IS_SPECIALIZATION_H_
#
# include <type_traits>

namespace preview {

template<typename Test, template<typename...> class Ref>
struct is_specialization : std::false_type {};

template<template<typename...> class Ref, typename... Args>
struct is_specialization<Ref<Args...>, Ref> : std::true_type {};

} // namespace preview

# endif // PREVIEW_TYPE_TRAITS_IS_SPECIALIZATION_H_
