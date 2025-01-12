add_executable(beat beat.cpp)

target_sources(beat PRIVATE beat.hpp)

if(WIN32)
  target_sources(beat PRIVATE resource/beat.rc resource/beat.Manifest)
endif()

target_enable_subproject(beat "beat (binary patching tool)")

target_include_directories(beat PRIVATE ${CMAKE_SOURCE_DIR})

target_link_libraries(beat PRIVATE nall ares::hiro)

ares_configure_executable(beat)
set_target_properties(beat PROPERTIES FOLDER tools PREFIX "")
