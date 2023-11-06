////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Implementation of common YART helper utility macros
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once


#include <cassert>
#include <cstdio>


////////////////////////////////////////////////////////////////////////////////////////////////////
// Helper Macros & Definitions
////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef DOXYGEN_EXCLUDE // Exclude from documentation
    #define YART_STRINGIFY(x) #x

    #define YART_SUPPRESS_PUSH(code) \
        _Pragma("warning(push)")     \
        _Pragma(YART_STRINGIFY(warning(disable: ## code)))

    #define YART_SUPPRESS_POP() \
        _Pragma("warning(pop)") 

    #define YART_ARRAYSIZE(arr) (sizeof(arr) / sizeof(*(arr)))
    #define YART_UNUSED(...) (void)sizeof(__VA_ARGS__)

    #define YART_ASSERT(expr) assert(expr) 
    #define YART_ABORT(msg) assert(0 && msg)
    #define YART_UNREACHABLE() YART_ABORT("Reached unreachable section")

    #define YART_LOG_ERR(format, ...) fprintf(stderr, format, __VA_ARGS__)
#endif // #ifndef DOXYGEN_EXCLUDE

// TODO: Create YART logging macros
