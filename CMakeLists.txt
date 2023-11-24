cmake_minimum_required(VERSION 3.14...3.27)

project(
  ares
  VERSION 0.134
  DESCRIPTION "ares is a cross-platform, open source, multi-system emulator, focusing on accuracy and preservation."
  LANGUAGES C CXX)

if(WIN32 AND NOT MINGW)
  add_compile_definitions(EXCLUDE_MANIFEST_FROM_RC) #global
endif()

function(add_sourcery_command target subdir)
  add_custom_command(OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/${subdir}/resource.cpp ${CMAKE_CURRENT_SOURCE_DIR}/${subdir}/resource.hpp
    COMMAND sourcery resource.bml resource.cpp resource.hpp
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${subdir}
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${subdir}/resource.bml
    VERBATIM
  )
  add_custom_target(${target}-resource DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${subdir}/resource.cpp ${CMAKE_CURRENT_SOURCE_DIR}/${subdir}/resource.hpp)
  add_dependencies(${target} ${target}-resource)
endfunction()

add_subdirectory(thirdparty)

set(build optimized)
set(threaded TRUE)
set(openmp FALSE)
set(vulkan TRUE)
set(local TRUE)
set(lto TRUE)
set(console FALSE)

set(default_build_type "Release")
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
  set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE
      STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
    "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

#for now, derive build type from cmake standard build type
set(build $<$<CONFIG:Debug>:debug>$<$<CONFIG:Release>:optimized>$<$<CONFIG:RelWithDebInfo>:release>$<$<CONFIG:MinSizeRel>:minified>)
#file(GENERATE OUTPUT build_$<CONFIG>.txt CONTENT "build = ${build}")

include_directories(.) #global

add_subdirectory(nall)

if(arch STREQUAL x86 OR arch STREQUAL amd64)
  if(NOT MSVC)
    if(local)
      add_compile_options(-march=native) #global
    else()
      # For official builds, default to x86-64-v2 (Intel Nehalem, AMD Bulldozer) which supports up to SSE 4.2.
      add_compile_options(-march=x86-64-v2) #global
    endif()
  endif()
endif()

add_subdirectory(libco)
add_subdirectory(ruby)

#hiro.resource := resource/ares.rc
add_subdirectory(hiro)

set(profile performance)
set(cores n64 ng spec)
#set(cores a26 fc sfc n64 sg ms md ps1 pce ng msx cv myvision gb gba ws ngp spec) #saturn

add_subdirectory(ares)
add_subdirectory(mia)
add_subdirectory(desktop-ui)
add_subdirectory(tests/i8080)
if(NOT WIN32)
  add_subdirectory(tools/genius)
endif()
add_subdirectory(tools/mame2bml)
add_subdirectory(tools/sourcery)


set_target_properties(
  ares
  hiro
  libco
  mia
  nall
  ruby
  desktop-ui
  libchdr
  sljit
  tzxfile
  ymfm
  PROPERTIES VS_GLOBAL_VcpkgEnabled false)
