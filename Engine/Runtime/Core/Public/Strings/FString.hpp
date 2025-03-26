/******************************************************
 * @file Strings/FString.hpp
 * @brief
 *****************************************************/

#pragma once
#define CORESTRING_CLANGD_USED // 用来应付Clangd无休无止的报错

#include "Strings/TemplateString.hpp"

#define TE_STRING_CLASS FString
#define TE_STRING_TYPE TCHAR

#include "Strings/TemplateString.inl"

#undef TE_STRING_CLASS
#undef TE_STRING_TYPE