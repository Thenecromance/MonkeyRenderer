#pragma once

#include <cstddef>

#include "assertMarco.inl"
#include "nspMarco.inl"
#include "sizeMarco.inl"

#if defined(_MSC_VER)
#if defined(__cplusplus)
#define DLL_EXPORT extern "C" __declspec(dllexport)
#define DLL_IMPORT extern "C" __declspec(dllimport)
#else
#define DLL_EXPORT __declspec(dllexport)
#define DLL_IMPORT __declspec(dllimport)
#endif
#endif  // defined(_MSC_VER)


#define USING(nspc) using namespace nspc;