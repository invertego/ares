add_library(ruby STATIC ruby.cpp)

target_link_libraries(ruby PUBLIC nall PRIVATE thirdparty)


if(NOT ruby)
  if(platform STREQUAL windows)
    list(APPEND ruby video.direct3d9 video.gdi)
    if (arch STREQUAL x86 OR arch STREQUAL amd64)
      list(APPEND ruby video.wgl)
    endif()
    list(APPEND ruby audio.wasapi audio.xaudio2 audio.directsound audio.waveout) #audio.asio
    list(APPEND ruby input.windows)

#  ifeq ($(call which,pkg-config),)
#    # TODO: Check presence of libSDL
#  else()
#    # If we're in a posix shell, use pkg-config/pkg-check
#    pkg_check = $(if $(shell pkg-config $1 && echo 1),$2)
#    ruby += $(call pkg_check,sdl2,input.sdl)
#    ruby += $(call pkg_check,sdl2,audio.sdl)
#  endif()
  elseif(platform STREQUAL macos)
    list(APPEND ruby video.cgl)
    list(APPEND ruby audio.openal)
    list(APPEND ruby input.quartz) #input.carbon
  elseif(platform STREQUAL linux)
#    pkg_check = $(if $(shell pkg-config $1 && echo 1),$2)
    list(APPEND ruby video.glx video.glx2 video.xshm)
#    ruby += $(call pkg_check,xv,video.xvideo)
    list(APPEND ruby audio.oss audio.alsa)
#    ruby += $(call pkg_check,openal,audio.openal)
#    ruby += $(call pkg_check,libpulse,audio.pulseaudio)
#    ruby += $(call pkg_check,libpulse-simple,audio.pulseaudiosimple)
#    ruby += $(call pkg_check,ao,audio.ao)
    list(APPEND ruby input.xlib)
#    ruby += $(call pkg_check,libudev,input.udev)
#    ruby += $(call pkg_check,sdl2,input.sdl)
#    ruby += $(call pkg_check,sdl2,audio.sdl)
  elseif(platform STREQUAL bsd)
#    pkg_check = $(if $(shell pkg-config $1 && echo 1),$2)
    list(APPEND ruby video.glx video.glx2 video.xshm)
#    ruby += $(call pkg_check,xv,video.xvideo)
    list(APPEND ruby audio.oss)
#    ruby += $(call pkg_check,openal,audio.openal)
#    ruby += $(call pkg_check,libpulse,audio.pulseaudio)
#    ruby += $(call pkg_check,libpulse-simple,audio.pulseaudiosimple)
#    ruby += $(call pkg_check,ao,audio.ao)
    list(APPEND ruby input.uhid input.xlib)
#    ruby += $(call pkg_check,sdl2,input.sdl)
#    ruby += $(call pkg_check,sdl2,audio.sdl)
  endif()
endif()

if(platform STREQUAL macos)
#  ruby.flags := $(flags.objcpp)
else()
#  ruby.flags := $(flags.cpp)
endif()

foreach(c IN LISTS ruby)
  target_compile_definitions(ruby PRIVATE $<MAKE_C_IDENTIFIER:$<UPPER_CASE:${c}>>)
endforeach()

#ifeq ($(call which,pkg-config),)
#  # TODO: add SDL2 cflags
#else()
#  ruby.flags += $(if $(findstring input.sdl,$(ruby)),$(shell pkg-config sdl2 --cflags))
#  ruby.flags += $(if $(findstring audio.sdl,$(ruby)),$(shell pkg-config sdl2 --cflags))
#endif()

set(ruby.options "")

#ruby.options += $(if $(findstring video.cgl,$(ruby)),-framework OpenGL)
if(video.direct3d9 IN_LIST ruby)
  target_link_libraries(ruby PRIVATE d3d9)
endif()
#ruby.options += $(if $(findstring video.glx,$(ruby)),-lGL)
if(video.wgl IN_LIST ruby)
  target_link_libraries(ruby PRIVATE opengl32)
endif()
#ruby.options += $(if $(findstring video.xvideo,$(ruby)),-lXv)

#ruby.options += $(if $(findstring audio.alsa,$(ruby)),-lasound)
#ruby.options += $(if $(findstring audio.ao,$(ruby)),-lao)
if(audio.directsound IN_LIST ruby)
  target_link_libraries(ruby PRIVATE dsound uuid)
endif()
#ruby.options += $(if $(findstring audio.pulseaudio,$(ruby)),-lpulse)
#ruby.options += $(if $(findstring audio.pulseaudiosimple,$(ruby)),-lpulse-simple)
if(audio.wasapi IN_LIST ruby)
  target_link_libraries(ruby PRIVATE avrt uuid)
endif()
if(audio.waveout IN_LIST ruby)
  target_link_libraries(ruby PRIVATE winmm)
endif()
if(audio.xaudio2 IN_LIST ruby)
  target_link_libraries(ruby PRIVATE ole32)
endif()

if(platform STREQUAL windows)
#  ifeq ($(call which,pkg-config),)
#    # TODO: add SDL2 ldflags
#  else()
#    ruby.options += $(if $(findstring input.sdl,$(ruby)),$(shell pkg-config sdl2 --libs --static))
#    ruby.options += $(if $(findstring audio.sdl,$(ruby)),$(shell pkg-config sdl2 --libs --static))
#  endif()
else()
#  ruby.options += $(if $(findstring input.sdl,$(ruby)),$(shell pkg-config sdl2 --libs))
#  ruby.options += $(if $(findstring audio.sdl,$(ruby)),$(shell pkg-config sdl2 --libs))
endif()

#ruby.options += $(if $(findstring input.udev,$(ruby)),-ludev)
#ruby.options += $(if $(findstring input.uhid,$(ruby)),-lusbhid)
if(input.windows IN_LIST ruby)
  target_link_libraries(ruby PRIVATE dinput8 dxguid)
endif()

if(platform STREQUAL windows)
  if(audio.openal IN_LIST ruby)
    target_link_libraries(ruby PRIVATE openal32)
  endif()
endif()

if(platform STREQUAL macos)
#  ruby.options += -framework IOKit
#  ruby.options += $(if $(findstring audio.openal,$(ruby)),-framework OpenAL)
endif()

if(platform STREQUAL linux)
#  ruby.options += -lX11 -lXext -lXrandr
#  ruby.options += $(if $(findstring audio.openal,$(ruby)),-lopenal)
endif()

if(platform STREQUAL bsd)
#  ruby.options += -lX11 -lXext -lXrandr
#  ruby.options += $(if $(findstring audio.openal,$(ruby)),-lopenal -fuse-ld=bfd)
#  # -fuse-ld=bfd: see FreeBSD bug 219089
endif()

#ruby.objects := $(object.path)/ruby.o

#$(object.path)/ruby.o: $(ruby.path)/ruby.cpp $(call rwildcard,$(ruby.path))
#	$(info Compiling $(subst ../,,$<) ...)
#	@$(compiler) $(ruby.flags) $(flags) $(flags.deps) -c $< $(call obj,$@)

#ruby.verbose:
#	$(info ruby Drivers:)
#	$(foreach n,$(ruby),$(info $([space]) $n))
