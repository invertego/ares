add_executable(desktop-ui
  desktop-ui.cpp
  resource/resource.cpp
  input/input.cpp
  emulator/emulator.cpp
  game-browser/game-browser.cpp
  program/program.cpp
  presentation/presentation.cpp
  settings/settings.cpp
  tools/tools.cpp
)

target_link_libraries(desktop-ui PRIVATE nall ruby hiro ares mia)

set_target_properties(
  desktop-ui
  PROPERTIES
  OUTPUT_NAME ares
)

add_sourcery_command(desktop-ui resource)

if(WIN32)
  target_sources(desktop-ui PRIVATE
    resource/ares.rc
    resource/ares.Manifest
  )
endif()

nall_set_properties(desktop-ui)
