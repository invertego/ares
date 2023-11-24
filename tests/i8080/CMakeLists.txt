add_executable(i8080
	i8080.cpp
	../../ares/component/processor/i8080/i8080.cpp
)

target_link_libraries(i8080 PRIVATE nall ares)

nall_set_properties(i8080)
