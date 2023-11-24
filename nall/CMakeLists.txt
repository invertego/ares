add_library(nall OBJECT
  nall.cpp
  main.cpp
  ../thirdparty/sljitAllocator.cpp
)

target_link_libraries(nall PUBLIC sljit libchdr)


# platform detection
if(NOT platform)
  if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(platform windows)
  elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set(platform macos)
  elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(platform linux)
  elseif(CMAKE_SYSTEM_NAME MATCHES "BSD")
    set(platform bsd)
  else()
    message(FATAL_ERROR "unknown platform, please specify manually.")
  endif()
endif()

# compiler detection
if(NOT compiler)
  if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(compiler cl)
  elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(compiler g++)
  elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(compiler clang++)
  else()
    message(FATAL_ERROR "unknown compiler, please specify manually.")
  endif()
endif()

if (CMAKE_SYSTEM_PROCESSOR MATCHES "^(x86|i[3-6]86)$")
  set(machine x86)
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(x86_64|amd64|AMD64)$")
  set(machine amd64)
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(aarch64|arm64|ARM64)$")
  set(machine arm64)
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(armv7l?|arm|ARM)$")
  set(machine arm32)
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(ppc64|ppc64le)")
  set(machine ppc64)
elseif(CMAKE_SYSTEM_PROCESSOR STREQUAL "riscv64")
  set(machine rv64)
elseif(CMAKE_SYSTEM_PROCESSOR STREQUAL "riscv32")
  set(machine rv32)
endif()

# global compiler flags
target_compile_features(nall PUBLIC c_std_11)
set_target_properties(nall PROPERTIES C_EXTENSIONS OFF)

target_compile_features(nall PUBLIC cxx_std_17)
set_target_properties(nall PROPERTIES CXX_EXTENSIONS OFF)

if(MSVC)
  target_compile_options(nall PUBLIC /W2)
endif()

# explicit architecture flags to allow for cross-compilation on macos
if(platform STREQUAL macos)
  if(arch STREQUAL amd64)
    add_compile_options(-arch x86_64) #global
    add_link_options(-arch x86_64) #global
  elseif(arch STREQUAL arm64)
    add_compile_options(-arch arm64) #global
    add_link_options(-arch arm64) #global
  endif()
  if(NOT machine STREQUAL arch)
    set(local FALSE)
  endif()
endif()

# architecture detection
if(NOT arch)
  if(machine)
    set(arch ${machine})
  else()
    message(FATAL_ERROR "unknown arch, please specify manually.")
  endif()
endif()

# build optimization levels
#ifeq ($(build),debug)
#  symbols = true
#  ifeq ($(cl),true)
#    flags += -Od
#  else
#    flags += -Og
#  endif
#  flags += -DBUILD_DEBUG
#else ifeq ($(build),stable)
#  flags += -O1 -DBUILD_STABLE
#else ifeq ($(build),minified)
#  flags += -Os -DBUILD_MINIFIED
#else ifeq ($(build),release)
#  flags += -O2 -DBUILD_RELEASE
#else ifeq ($(build),optimized)
#  ifeq ($(cl),true)
#    flags += -O2
#  else
#    flags += -O3 -fomit-frame-pointer
#  endif
#  flags += -DBUILD_OPTIMIZED
#else
#  $(error unrecognized build type.)
#endif

target_compile_definitions(nall PUBLIC BUILD_$<UPPER_CASE:${build}>)

if(local)
  target_compile_definitions(nall PUBLIC BUILD_LOCAL)
endif()

# debugging information
#ifeq ($(symbols),true)
#  ifeq ($(cl),true)
#    flags += -Zi -FS
#    options += -debug
#    ifneq ($(build),debug)
#      options += -opt:ref,icf
#    endif
#  else
#    flags += -g
#    ifeq ($(platform),windows)
#      ifeq ($(findstring clang++,$(compiler)),clang++)
#        ifeq ($(symformat),gdb)
#          flags += -ggdb
#        else  
#          flags += -gcodeview
#        endif
#        ifeq ($(msvc),true)
#          options += -Wl,-debug
#        else
#          options += -Wl,-pdb=
#        endif
#      endif
#    endif
#  endif
#endif

# link-time optimization
#ifeq ($(lto),true)
#  ifeq ($(cl),true)
#    ifneq ($(findstring clang,$(compiler)),clang)
#      flags   += -GL
#      options += -ltcg:incremental -ltcgout:$(object.path)/$(name).iobj
#    else
#      flags   += -flto=thin
#      options += -lldltocache:$(object.path)/lto
#    endif
#  else
#    ifneq ($(findstring clang++,$(compiler)),clang++)
#      flags   += -flto=auto -fno-fat-lto-objects
#    else
#      flags   += -flto=thin
#      options += -flto=thin
#      ifeq ($(platform),macos)
#        options += -Wl,-cache_path_lto,$(object.path)/lto
#      else ifeq ($(msvc),true)
#        options += -Wl,-lldltocache:$(object.path)/lto
#      else
#        options += -Wl,--thinlto-cache-dir=$(object.path)/lto
#      endif
#    endif
#  endif
#endif

# openmp support
#ifeq ($(openmp),true)
#  # macOS Xcode does not ship with OpenMP support
#  ifneq ($(platform),macos)
#    flags   += -fopenmp
#    options += -fopenmp
#  endif
#endif

# clang settings
if(compiler STREQUAL clang++)
  target_compile_options(nall PUBLIC -fno-strict-aliasing -fwrapv)
  if(NOT platform STREQUAL macos)
    target_link_options(nall PUBLIC -fuse-ld=lld)
  endif()
# gcc settings
elseif(compiler STREQUAL g++)
  target_compile_options(nall PUBLIC -fno-strict-aliasing -fwrapv -Wno-trigraphs)
endif()

# windows settings
if(platform STREQUAL windows)
  # target Windows 7
  add_compile_definitions(_WIN32_WINNT=0x0601) #global
  if(NOT MINGW)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS _CRT_NONSTDC_NO_WARNINGS) #global
  endif()
  target_link_libraries(nall PUBLIC ws2_32 ole32 shell32 shlwapi)
  if(MINGW)
    target_link_options(nall PUBLIC -mthreads -static)
  endif()
  if(console)
    target_compile_definitions(nall PUBLIC SUBSYTEM_CONSOLE)
  else()
    target_compile_definitions(nall PUBLIC SUBSYTEM_WINDOWS)
  endif()
endif()

function(nall_set_properties target)
  set_target_properties(${target} PROPERTIES WIN32_EXECUTABLE $<NOT:$<BOOL:${console}>>)
endfunction()

# macos settings
#ifeq ($(platform),macos)
#  flags   += -stdlib=libc++ -mmacosx-version-min=10.9 -Wno-auto-var-id -fobjc-arc
#  options += -lc++ -lobjc -mmacosx-version-min=10.9
#  # allow mprotect() on dynamic recompiler code blocks
#  options += -Wl,-segprot,__DATA,rwx,rw
#endif

# linux settings
#ifeq ($(platform),linux)
#  options += -ldl
#endif

# bsd settings
#ifeq ($(platform),bsd)
#  flags   += -I/usr/local/include
#  options += -Wl,-rpath=/usr/local/lib
#  options += -Wl,-rpath=/usr/local/lib/gcc8
#  options += -lstdc++ -lm
#endif

# threading support
#ifeq ($(threaded),true)
#  ifneq ($(filter $(platform),linux bsd),)
#    flags   += -pthread
#    options += -pthread -lrt
#  endif
#endif

#nall.verbose:
#	$(info Compiler:)
#	$(info $([space]) $(compiler))
#	$(info Compiler Flags:)
#	$(foreach n,$(sort $(call unique,$(flags))),$(if $(filter-out -I%,$n),$(info $([space]) $n)))
#	$(info Linker Options:)
#	$(foreach n,$(sort $(call unique,$(options))),$(if $(filter-out -l%,$n),$(info $([space]) $n)))

set(platform ${platform} PARENT_SCOPE)
set(compiler ${compiler} PARENT_SCOPE)
set(machine ${machine} PARENT_SCOPE)
set(local ${local} PARENT_SCOPE)
set(arch ${arch} PARENT_SCOPE)

message("platform ${platform}")
message("compiler ${compiler}")
message("machine ${machine}")
message("local ${local}")
message("arch ${arch}")
