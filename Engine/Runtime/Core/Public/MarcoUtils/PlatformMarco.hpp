/******************************************************
 * @file MarcoUtils/PlatformMarco.hpp
 * @brief 平台相关宏
 *****************************************************/

#pragma once

// Platform detection
// clang-format off
#if defined(_WIN32) || defined(_WIN64)
    #define ENGINE_PLATFORM_WINDOWS 1
    #if defined(_WIN64)
        #define ENGINE_PLATFORM_WINDOWS_64 1
    #else
        #define ENGINE_PLATFORM_WINDOWS_32 1
    #endif
#elif defined(__APPLE__) && defined(__MACH__)
    #include <TargetConditionals.h>
    #define ENGINE_PLATFORM_APPLE 1
    #if TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR
        #define ENGINE_PLATFORM_IOS 1
    #else
        #define ENGINE_PLATFORM_MACOS 1
    #endif
#elif defined(__linux__)
    #define ENGINE_PLATFORM_LINUX 1
#elif defined(__ANDROID__)
    #define ENGINE_PLATFORM_ANDROID 1
#else
    #error "Unknown platform"
#endif


#ifdef ENGINE_PLATFORM_WINDOWS
    #define NOTHING
#elif ENGINE_PLATFORM_LINUX
    #include <linux/version.h>
    #include <signal.h>

    
    #ifdef BUILD_DEBUG
        #define FORCEINLINE inline
    #else
        #define FORCEINLINE inline __attribute__ ((always_inline))
    #endif	
    #define PLATFORM_BREAK()	raise(SIGTRAP)   
    
    //平台类型
    struct FPlatformTypes {
        using TCHAR = char16_t;
    };
#endif

/** Branch prediction hints */				
#if ( defined(__clang__) || defined(__GNUC__) ) && (ENGINE_PLATFORM_LINUX)	
    #define LIKELY(x)			__builtin_expect(!!(x), 1)
#else
    #define LIKELY(x)			(!!(x))
#endif

#if ( defined(__clang__) || defined(__GNUC__) ) && (ENGINE_PLATFORM_LINUX)
    #define UNLIKELY(x)			__builtin_expect(!!(x), 0)
#else
    #define UNLIKELY(x)			(!!(x))
#endif
// clang-format on
