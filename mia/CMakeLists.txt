add_library(mia STATIC mia.cpp resource/resource.cpp)

target_compile_definitions(mia PUBLIC MIA_LIBRARY)

target_link_libraries(mia PUBLIC nall ares PRIVATE tzxfile)

add_sourcery_command(mia resource)


add_executable(mia-standalone mia.cpp resource/resource.cpp)

target_link_libraries(mia-standalone PUBLIC nall hiro ares PRIVATE tzxfile)

add_sourcery_command(mia-standalone resource)

if(WIN32)
  target_sources(mia-standalone PRIVATE
    resource/mia.rc
    resource/mia.Manifest
  )
endif()

nall_set_properties(mia-standalone)
