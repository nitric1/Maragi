﻿#pragma once

#include <cstdint>
#include <climits>

#ifndef LONG32_T_DEFINED
#  if ((LONG_MAX - 4) <= 2147483647) // 32-bit long type
typedef signed long long32_t;
typedef int64_t long64_t;

typedef unsigned long ulong32_t;
typedef uint64_t ulong64_t;
#  else // 64-bit long type
typedef int32_t long32_t;
typedef signed long long64_t;

typedef uint32_t ulong32_t;
typedef unsigned long ulong64_t;
#  endif

#  if ((SIZE_MAX - 4) <= 4294967295) // 32-bit platform
#    ifndef BATANG_W64
#      if defined(_MSC_VER) && _MSC_VER >= 1300 && !defined(__midl) && (defined(_X86_) || defined(_M_IX86))
#        define BATANG_W64 __w64
#      else
#        define BATANG_W64
#      endif
#    endif
typedef BATANG_W64 long32_t longptr_t;
typedef BATANG_W64 ulong32_t ulongptr_t;
#    ifdef BATANG_W64
#      undef BATANG_W64
#    endif
#  else
typedef long64_t longptr_t;
typedef ulong64_t ulongptr_t;
#  endif

#  define LONG32_T_DEFINED
#endif
