//
// Created by yonggyulee on 2024/01/04.
//

#ifndef PREVIEW_CORE_NO_UNIQUE_ADDRESS_H_
#define PREVIEW_CORE_NO_UNIQUE_ADDRESS_H_

#if __cplusplus < 202002L
#define PREVIEW_NO_UNIQUE_ADDRESS
#else
#define PREVIEW_NO_UNIQUE_ADDRESS [[no_unique_address]]
#endif

#endif // PREVIEW_CORE_NO_UNIQUE_ADDRESS_H_
