add_library(hiro STATIC hiro.cpp)

target_link_libraries(hiro PUBLIC nall)

#PKG_CONFIG ?= pkg-config


if(platform STREQUAL windows)
  if(NOT hiro)
    set(hiro windows)
  endif()

  if(hiro STREQUAL windows)
    target_compile_definitions(hiro PRIVATE HIRO_WINDOWS)
    target_link_libraries(hiro PRIVATE kernel32 user32 gdi32 advapi32 ole32 comctl32 comdlg32 uxtheme msimg32 dwmapi)
  endif()

  if(hiro STREQUAL gtk2)
#    hiro.flags   = $(flags.cpp) -DHIRO_GTK=2 $(shell pkg-config --cflags gtk+-2.0) -Wno-deprecated-declarations
#    hiro.options = $(shell pkg-config --libs gtk+-2.0)
  endif()

  if(hiro STREQUAL gtk3)
#    hiro.flags   = $(flags.cpp) -DHIRO_GTK=3 $(shell pkg-config --cflags gtk+-3.0) -Wno-deprecated-declarations
#    hiro.options = $(shell pkg-config --libs gtk+-3.0)
  endif()
endif()

if(platform STREQUAL macos)
  if(NOT hiro)
    set(hiro cocoa)
  endif()

  if(hiro STREQUAL cocoa)
#    hiro.flags   = $(flags.objcpp) -w -DHIRO_COCOA
#    hiro.options = -framework Cocoa -framework Carbon -framework IOKit -framework Security
  endif()
endif()

if(platform STREQUAL linux OR platform STREQUAL bsd)
  if(NOT hiro)
    set(hiro gtk3)
  endif()

  if(hiro STREQUAL gtk2)
#    hiro.flags   = $(flags.cpp) -DHIRO_GTK=2 $(shell $(PKG_CONFIG) --cflags gtk+-2.0) -Wno-deprecated-declarations
#    hiro.options = -L/usr/local/lib -lX11 $(shell $(PKG_CONFIG) --libs gtk+-2.0)
  endif()

  if(hiro STREQUAL gtk2-se)
#    flags       += -DHiro_SourceEdit
#    hiro.flags   = $(flags.cpp) -DHIRO_GTK=2 $(shell $(PKG_CONFIG) --cflags gtk+-2.0 gtksourceview-2.0) -Wno-deprecated-declarations
#    hiro.options = -L/usr/local/lib -lX11 $(shell $(PKG_CONFIG) --libs gtk+-2.0 gtksourceview-2.0)
  endif()

  if(hiro STREQUAL gtk3)
#    hiro.flags   = $(flags.cpp) -DHIRO_GTK=3 $(shell $(PKG_CONFIG) --cflags gtk+-3.0) -Wno-deprecated-declarations
#    hiro.options = -L/usr/local/lib -lX11 $(shell $(PKG_CONFIG) --libs gtk+-3.0)
  endif()

  if(hiro STREQUAL gtk3-se)
#    flags       += -DHiro_SourceEdit
#    hiro.flags   = $(flags.cpp) -DHIRO_GTK=3 $(shell $(PKG_CONFIG) --cflags gtk+-3.0 gtksourceview-3.0) -Wno-deprecated-declarations
#    hiro.options = -L/usr/local/lib -lX11 $(shell $(PKG_CONFIG) --libs gtk+-3.0 gtksourceview-3.0)
  endif()

  if(hiro STREQUAL qt4)
#    moc = /usr/local/lib/qt4/bin/moc
#    hiro.flags   = $(flags.cpp) -DHIRO_QT=4 $(shell $(PKG_CONFIG) --cflags QtCore QtGui)
#    hiro.options = -L/usr/local/lib -lX11 $(shell $(PKG_CONFIG) --libs QtCore QtGui)
  endif()

  if(hiro STREQUAL qt5)
#    moc = $(shell $(PKG_CONFIG) --variable=host_bins Qt5Core)/moc
#    hiro.flags   = $(flags.cpp) -DHIRO_QT=5 -fPIC $(shell $(PKG_CONFIG) --cflags Qt5Core Qt5Gui Qt5Widgets)
#    hiro.options = -L/usr/local/lib -lX11 $(shell $(PKG_CONFIG) --libs Qt5Core Qt5Gui Qt5Widgets)
  endif()
endif()

if(NOT hiro.resource)
  set(hiro.resource windows/hiro.rc)
endif()

#hiro.objects := \
#  $(object.path)/hiro-$(hiro).o \
#  $(if $(filter windows,$(hiro)),$(object.path)/hiro-resource$(hiro.resource.extension))

#$(object.path)/hiro-$(hiro).o: $(hiro.path)/hiro.cpp
#	$(if $(filter qt%,$(hiro)),$(info Compiling $(hiro.path)/qt/qt.moc ...))
#	$(if $(filter qt%,$(hiro)),@$(moc) -i -o $(hiro.path)/qt/qt.moc $(hiro.path)/qt/qt.hpp)
#	$(info Compiling $(subst ../,,$<) ...)
#	@$(compiler) $(hiro.flags) $(flags) $(flags.deps) -c $< $(call obj,$@)

#$(object.path)/hiro-resource$(hiro.resource.extension): $(hiro.resource)
#	$(info Compiling $(subst ../,,$<) ...)
#	@$(windres) $(call hiro.resource.command,$<,$@)

#hiro.verbose:
#	$(info hiro Target:)
#	$(info $([space]) $(hiro))

add_sourcery_command(hiro resource)
