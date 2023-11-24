add_executable(mame2bml mame2bml.cpp)

target_link_libraries(mame2bml PRIVATE nall)

nall_set_properties(mame2bml)
