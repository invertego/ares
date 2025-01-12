name := beat
build := optimized
flags += -I../..

nall.path := ../../nall
include $(nall.path)/GNUmakefile

hiro.path := ../../hiro
hiro.resource := resource/$(name).rc
include $(hiro.path)/GNUmakefile

objects := $(object.path)/$(name).o
$(object.path)/$(name).o: $(name).cpp

all.objects := $(nall.objects) $(hiro.objects) $(objects)
all.options := $(nall.options) $(hiro.options) $(options)

$(all.objects): | $(object.path)

all: $(all.objects) | $(output.path)
	$(info Linking $(output.path)/$(name)$(extension) ...)
	+@$(compiler) -o $(output.path)/$(name)$(extension) $(all.objects) $(all.options)
ifeq ($(platform),macos)
	rm -rf $(output.path)/$(name).app
	mkdir -p $(output.path)/$(name).app/Contents/MacOS/
	mkdir -p $(output.path)/$(name).app/Contents/Resources/
	mv $(output.path)/$(name) $(output.path)/$(name).app/Contents/MacOS/$(name)
	cp resource/$(name).plist $(output.path)/$(name).app/Contents/Info.plist
	sips -s format icns resource/$(name).png --out $(output.path)/$(name).app/Contents/Resources/$(name).icns
endif

verbose: hiro.verbose nall.verbose all;

clean:
ifeq ($(platform),macos)
	rm -rf $(output.path)/$(name).app
endif
	$(call delete,$(object.path)/*)
	$(call delete,$(output.path)/*)

install: all
ifeq ($(shell id -un),root)
	$(error "make install should not be run as root")
else ifeq ($(platform),macos)
	cp -R $(output.path)/$(name).app /Applications/$(name).app
else ifneq ($(filter $(platform),linux bsd),)
	mkdir -p $(prefix)/bin/
	mkdir -p $(prefix)/share/applications/
	mkdir -p $(prefix)/share/icons/
	cp $(output.path)/$(name) $(prefix)/bin/$(name)
	cp resource/$(name).desktop $(prefix)/share/applications/$(name).desktop
	cp resource/$(name).png $(prefix)/share/icons/$(name).png
endif

uninstall:
ifeq ($(shell id -un),root)
	$(error "make uninstall should not be run as root")
else ifeq ($(platform),macos)
	rm -rf /Applications/$(name).png
else ifneq ($(filter $(platform),linux bsd),)
	rm -f $(prefix)/bin/$(name)
	rm -f $(prefix)/share/applications/$(name).desktop
	rm -f $(prefix)/share/icons/$(name).png
endif

-include $(object.path)/*.d
