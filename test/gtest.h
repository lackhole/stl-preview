//
// Created by YongGyu Lee on 2024. 6. 15..
//

#ifndef PREVIEW_TEST_GTEST_H_
#define PREVIEW_TEST_GTEST_H_

#include <gtest/gtest.h>

#include "preview/__core/std_version.h"

#define VERSION_CAT2(x, y) x ## _CXXSTD_ ## y
#define VERSION_CAT(x, y) VERSION_CAT2(x, y)

#define VERSIONED(name) VERSION_CAT(name, PREVIEW_CXX_VERSION)

#endif //PREVIEW_TEST_GTEST_H_
