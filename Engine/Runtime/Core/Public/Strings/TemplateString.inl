/******************************************************
 * @file Strings/TemplateString.inl
 * @brief
 *****************************************************/

// 仅仅用于静态分析
#ifndef CORESTRING_CLANGD_USED
#include "Strings/CoreString.hpp"
#define TE_STRING_CLASS FString
#define TE_STRING_TYPE TCHAR

#endif

struct TE_JOIN(TE_STRING_CLASS, FormatArg); // struct FStringFormatArg
template <typename InKeyType, typename InValueType, typename SetAllocator,
          typename KeyFuncs>
class TMap;

using TE_JOIN(TE_STRING_CLASS, FormatNamedArguments) =
    TMap<TE_STRING_CLASS, PREPROCESSOR_JOIN(TE_STRING_CLASS, FormatArg)>
    // FStringFormatNamedArguments = TMap<FString, FStringFormatArg>