add_executable(sourcery sourcery.cpp)

target_link_libraries(sourcery PRIVATE nall)

nall_set_properties(sourcery)
