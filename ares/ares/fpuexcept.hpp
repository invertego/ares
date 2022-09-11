#pragma once

#include <nall/platform.hpp>
#include <nall/intrinsics.hpp>
#include <nall/maybe.hpp>

/*
 * Support for intercepting FPU exceptions.
 *
 * We implement different strategies:
 *   * FPE_HANDLER_VECTORED: use Windows Vectored Exception Handler.
 *   * FPE_HANDLER_SEH: use Windows Structured Exception Handler (__try / __except).
 *   * FPE_HANDLER_SIGNAL: use POSIX signals (SIGFPE) to intercept the exception, then disable
 *     it to continue and re-execute the offending instruction.
 *   * FPE_HANDLER_SIGNAL_SJLJ: use POSIX signals (SIGFPE) to intercept the exception, then
 *     use setjmp/longjmp to recover execution to a safe place and skip the offending instruction.
 * 
 * NOTE:
 *   * GCC doesn't support SEH (while LLVM does). We use VEH in this case.
 *   * macOS/ARM64 has broken FPE support in general. It generates a SIGILL instead of SIGFPE,
 *     and there is no way to know which kind of exception was triggered.
 */

#if 1
    #define FPE_HANDLER_TEST
#elif defined(PLATFORM_WINDOWS)
  #if defined(COMPILER_GCC)
    #define FPE_HANDLER_VECTORED
  #else
    #define FPE_HANDLER_SEH
  #endif
#elif defined(PLATFORM_LINUX) && defined(ARCHITECTURE_AMD64)
    #define FPE_HANDLER_SIGNAL
#elif defined(PLATFORM_MACOS) && defined(ARCHITECTURE_ARM64)
    #define FPE_HANDLER_SIGNAL
#else
    #define FPE_HANDLER_SIGNAL_SJLJ
#endif

#if defined(FPE_HANDLER_SIGNAL_SJLJ)
#include <setjmp.h>
#endif
#if defined(FPE_HANDLER_SIGNAL) || defined(FPE_HANDLER_SIGNAL_SJLJ)
#include <signal.h>
#endif

namespace fpe {
  namespace internal {
    extern volatile int raised_mask;
    extern volatile int saved_mask;
    inline u32 saved_status;
    inline u32 enable_mask;

    #if defined(FPE_HANDLER_SIGNAL_SJLJ)
    extern sigjmp_buf jmpbuf;
    #endif

    #if defined(FPE_HANDLER_VECTORED) || defined(FPE_HANDLER_SEH)
    auto exceptionFilter(u32 code) -> int;
    auto NTAPI vectoredExceptionHandler(EXCEPTION_POINTERS* info) -> LONG;
    #endif

    #if defined(ARCHITECTURE_AMD64)
    namespace mxcsr {
      inline auto read() -> u32 { return _mm_getcsr(); }
      inline auto write(u32 value) -> void { _mm_setcsr(value); }
    }
    #endif

    #if defined(ARCHITECTURE_ARM64)
    namespace fpsr {
      inline auto read() -> u32 {
        u64 fpsr;
        __asm__ __volatile__("mrs %0, FPSR" : "=r"(fpsr));
        return fpsr;
      }
      inline auto write(u32 value) -> void {
        u64 fpsr = value;
        __asm__ __volatile__("msr FPSR, %0" : : "r"(fpsr));
      }
    }
    #endif
  }

  auto install() -> void;
  auto uninstall() -> void;
  auto begin(int exc_mask) -> void;
  auto end(int exc_mask) -> void;

  inline auto readStatus() -> u32 {
    #if defined(ARCHITECTURE_AMD64)
    return internal::mxcsr::read();
    #elif defined(ARCHITECTURE_ARM64)
    return internal::fpsr::read();
    #else
    #error Unimplemented architecture
    #endif
  }

  inline auto writeStatus(u32 value) -> void {
    #if defined(ARCHITECTURE_AMD64)
    internal::mxcsr::write(value);
    #elif defined(ARCHITECTURE_ARM64)
    internal::fpsr::write(value);
    #else
    #error Unimplemented architecture
    #endif
  }

  inline auto clearStatus() -> void {
    writeStatus(readStatus() & ~FE_ALL_EXCEPT);
  }

  inline auto freeze() -> void {
    #if defined(FPE_HANDLER_TEST)
    internal::saved_status = readStatus();
    #endif
  }

  inline auto unfreeze() -> void {
    #if defined(FPE_HANDLER_TEST)
    writeStatus(internal::saved_status);
    #endif
  }
}

#if defined(FPE_HANDLER_TEST)
//#define CHECK_FPE(type, operation, exc_func) operation
#define CHECK_FPE(type, operation, exc_func) ({ \
  volatile type res = operation; \
  u32 raised_mask = fpe::readStatus() & fpe::internal::enable_mask; \
  if(raised_mask) { \
    /*exc_func(raised_mask);*/ \
    exception.floatingPoint(); \
    return; \
  } \
  (res); \
})
#define GUARD_FPE(operation) operation
#endif

#if defined(FPE_HANDLER_SEH)
#define CHECK_FPE(type, operation, exc_func) ({ \
  type res; \
  __try { \
    /* due to an LLVM limitation, it is not possible to catch an asynchronous */ \
    /* exception generated in the same frame as the catching __try. */ \
    res = [&] { return operation; }(); \
  } __except(fpe::internal::exceptionFilter(exception_code())) { \
    exc_func(fpe::internal::raised_mask); \
    return; \
  } \
  (res); \
})
#define GUARD_FPE(operation) ({ \
  __try { \
    [&] { operation; }(); \
  } __except(fpe::internal::vectoredExceptionHandler(exception_info())) { \
    abort(); \
  } \
})
#endif

#if defined(FPE_HANDLER_VECTORED) || defined(FPE_HANDLER_SIGNAL)
#define CHECK_FPE(type, operation, exc_func) ({ \
  fpe::internal::raised_mask = 0; \
  volatile type res = operation; \
  if(fpe::internal::raised_mask) { \
    exc_func(fpe::internal::raised_mask); \
    fpe::begin(fpe::internal::saved_mask); \
    return; \
  } \
  (res); \
})
#define GUARD_FPE(operation) operation
#endif

#if defined(FPE_HANDLER_SIGNAL_SJLJ)
#define FE_SJLJ     -1

#define CHECK_FPE(type, operation, exc_func) ({ \
  volatile type res; \
  fpe::internal::raised_mask = FE_SJLJ; \
  if(sigsetjmp(fpe::internal::jmpbuf, 1)) { \
    exc_func(fpe::internal::raised_mask); \
    fpe::internal::raised_mask = 0; \
    return; \
  } else { \
    res = operation; \
    fpe::internal::raised_mask = 0; \
  } \
  (res); \
})
#define GUARD_FPE(operation) operation
#endif
