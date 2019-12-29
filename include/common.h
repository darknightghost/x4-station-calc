#pragma once

/// Remove warnings of unused parameter.
#if ! defined(UNREFERENCED_PARAMETER)
    #define UNREFERENCED_PARAMETER(p) (p)
#endif // ! UNREFERENCED_PARAMETER

#include <common/compare.h>
#include <common/multi_threading.h>
