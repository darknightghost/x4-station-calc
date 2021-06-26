#pragma once

/// Remove warnings of unused parameter.
#if ! defined(UNREFERENCED_PARAMETER)
    #define UNREFERENCED_PARAMETER(p) (p)
#endif // ! UNREFERENCED_PARAMETER

#include <common/auto_release.h>
#include <common/compare.h>
#include <common/generic_reference.h>
//#include <common/generic_string.h>
#include <common/multi_threading.h>
#include <common/types.h>
