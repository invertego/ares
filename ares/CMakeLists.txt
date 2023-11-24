add_library(ares STATIC
  ares/ares.cpp
  ares/memory/fixed-allocator.cpp
)

target_include_directories(ares PUBLIC .)

target_link_libraries(ares
  PUBLIC libco sljit nall
  PRIVATE thirdparty ymfm
)

if(platform STREQUAL windows)
#  target_link_libraries(ares PRIVATE uuid kernel32 user32 gdi32 comctl32 comdlg32 shell32)
elseif(platform STREQUAL macos)
elseif(platform STREQUAL linux OR platform STREQUAL bsd)
#  target_link_options(ares PRIVATE LINKER:-export-dynamic)
#  target_link_libraries(ares PRIVATE X11 Xext)
else()
  message(FATAL_ERROR "ares is a library and cannot be built directly.")
endif()

if(platform STREQUAL macos)
#  set(ares.dylibs "")
endif()

if(vulkan)
  target_compile_definitions(ares PUBLIC VULKAN)
endif()

if(profile STREQUAL accuracy)
  target_compile_definitions(ares PUBLIC PROFILE_ACCURACY)
endif()

if(profile STREQUAL performance)
  target_compile_definitions(ares PUBLIC PROFILE_PERFORMANCE)
endif()

macro(ares_components)
  list(APPEND ares.components ${ARGV})
  set(ares.components ${ares.components} PARENT_SCOPE)
endmacro()

if(a26 IN_LIST cores)
  add_subdirectory(a26)
endif()

if(fc IN_LIST cores)
  add_subdirectory(fc)
endif()

if(sfc IN_LIST cores)
  add_subdirectory(sfc)
endif()

if(n64 IN_LIST cores)
  add_subdirectory(n64)
endif()

if(sg IN_LIST cores)
  add_subdirectory(sg)
endif()

if(ms IN_LIST cores)
  add_subdirectory(ms)
endif()

if(md IN_LIST cores)
  add_subdirectory(md)
endif()

if(saturn IN_LIST cores)
  add_subdirectory(saturn)
endif()

if(ps1 IN_LIST cores)
  add_subdirectory(ps1)
endif()

if(pce IN_LIST cores)
  add_subdirectory(pce)
endif()

if(msx IN_LIST cores)
  add_subdirectory(msx)
endif()

if(cv IN_LIST cores)
  add_subdirectory(cv)
endif()

if(myvision IN_LIST cores)
  add_subdirectory(myvision)
endif()

if(gb IN_LIST cores)
  add_subdirectory(gb)
endif()

if(gba IN_LIST cores)
  add_subdirectory(gba)
endif()

if(ws IN_LIST cores)
  add_subdirectory(ws)
endif()

if(ng IN_LIST cores)
  add_subdirectory(ng)
endif()

if(ngp IN_LIST cores)
  add_subdirectory(ngp)
endif()

if(spec IN_LIST cores)
  add_subdirectory(spec)
endif()

add_subdirectory(component)

foreach(c IN LISTS cores)
  target_compile_definitions(ares PUBLIC CORE_$<UPPER_CASE:${c}>)
endforeach()

add_sourcery_command(ares ares/resource)
