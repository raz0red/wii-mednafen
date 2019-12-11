#---------------------------------------------------------------------------------
# Clear the implicit built in rules
#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------
ifeq ($(strip $(DEVKITPPC)),)
$(error "Please set DEVKITPPC in your environment. export DEVKITPPC=<path to>devkitPPC)
endif

#include $(DEVKITPPC)/gamecube_rules
include $(DEVKITPPC)/wii_rules

#---------------------------------------------------------------------------------
# TARGET is the name of the output
# BUILD is the directory where object files & intermediate files will be placed
# SOURCES is a list of directories containing source code
# INCLUDES is a list of directories containing extra header files
#---------------------------------------------------------------------------------
TARGET      :=  boot
BUILD		:=	build

DATA		:=	\
    res/fonts \
    res/gfx
   
SOURCES		:=	\
    src/mednafen/src \
    src/mednafen/src/compress \
    src/mednafen/src/drivers \
    src/mednafen/src/sexyal \
    src/mednafen/src/sexyal/drivers \
    src/mednafen/src/sound \
    src/mednafen/src/string \
    src/mednafen/src/trio \
    src/mednafen/src/video \
    src/mednafen/src/gb \
    src/mednafen/src/gba \
    src/mednafen/src/lynx \
    src/mednafen/src/md \
    src/mednafen/src/md/cart \
    src/mednafen/src/md/cd \
    src/mednafen/src/md/input \
    src/mednafen/src/nes \
    src/mednafen/src/nes/boards \
    src/mednafen/src/nes/input \
    src/mednafen/src/nes/ntsc \
    src/mednafen/src/nes/ppu \
    src/mednafen/src/ngp \
    src/mednafen/src/ngp/TLCS-900h \
    src/mednafen/src/pce_fast \
    src/mednafen/src/pcfx \
    src/mednafen/src/pcfx/input \
    src/mednafen/src/sms \
    src/mednafen/src/vb \
    src/mednafen/src/wswan \
    src/mednafen/src/hw_cpu/c68k \
    src/mednafen/src/hw_cpu/v810 \
    src/mednafen/src/hw_cpu/v810/fpu-new \
    src/mednafen/src/hw_cpu/z80-fuse \
    src/mednafen/src/hw_sound/gb_apu \
    src/mednafen/src/hw_sound/pce_psg \
    src/mednafen/src/hw_sound/sms_apu \
    src/mednafen/src/hw_sound/ym2413 \
    src/mednafen/src/hw_sound/ym2612 \
    src/mednafen/src/hw_misc/arcade_card \
    src/mednafen/src/hw_video/huc6270 \
    src/mednafen/src/mpcdec \
    src/mednafen/src/cdrom \
    src/mednafen/src/tremor \
    src/wii \
    src/wii/emulator \
    src/wii/emulator/helper \
    src/wii/emulator/gb \
    src/wii/emulator/gba \
    src/wii/emulator/gg \
    src/wii/emulator/lynx \
    src/wii/emulator/md \
    src/wii/emulator/nes \
    src/wii/emulator/ngp \
    src/wii/emulator/vb \
    src/wii/emulator/pcefast \
    src/wii/emulator/pcfx \
    src/wii/emulator/wswan \
    src/wii/emulator/sms

INCLUDES	:= \
    wii-emucommon/include \
    wii-emucommon/netprint/include \
    wii-emucommon/pngu/include \
    wii-emucommon/FreeTypeGX/include \
    wii-emucommon/i18n/include \
    wii-emucommon/sdl/SDL/include \
    wii-emucommon/sdl/SDL_ttf/include \
    wii-emucommon/sdl/SDL_image/include \
    src/mednafen/include \
    src/mednafen/include/blip \
    src/mednafen/src/drivers \
    src/mednafen/src/md \
    src/mednafen/src/ngp \
    src/mednafen/src/ngp/TLCS-900h \
    src/mednafen/src/hw_sound \
    src/mednafen/src/hw_misc \
    src/mednafen/src/hw_cpu \
    src/mednafen/src/hw_video \
    src/wii \
    src/wii/emulator \
    src/wii/emulator/helper \
    src/wii/emulator/gb \
    src/wii/emulator/gba \
    src/wii/emulator/gg \
    src/wii/emulator/lynx \
    src/wii/emulator/md \
    src/wii/emulator/nes \
    src/wii/emulator/ngp \
    src/wii/emulator/vb \
    src/wii/emulator/pcefast \
    src/wii/emulator/pcfx \
    src/wii/emulator/wswan \
    src/wii/emulator/sms

#---------------------------------------------------------------------------------
# options for code generation
#---------------------------------------------------------------------------------  
#CFLAGS  -Wall -DWII_NETTRACE -DWII_NETTRACE -DTRACK_UNIQUE_MSGIDS 
#  -fomit-frame-pointer \
#  --param large-function-growth=800 \
#  --param inline-unit-growth=200 \
#  -Winline \
#  -Wno-strict-aliasing \


CFLAGS	= \
  -g -O3 $(MACHDEP) $(INCLUDE) -DHAVE_CONFIG_H -DHAVE_FLOAT_H -DMEM2 -fno-strict-overflow -fsigned-char \
  -Winline \
  -fomit-frame-pointer \
  --param large-function-growth=800 \
  --param inline-unit-growth=200 \
  -Wno-strict-aliasing \
  -Wno-narrowing \
  -DWII \
  -DVB_BPP=8 \
  -DWSWAN_BPP=16 \
  -DGBA_BPP=16 \
  -DPCE_FAST_BPP=16 \
  -DNES_BPP=16 \
  -DPCFX_BPP=16 \
  -DMD_BPP=16 \
  -DPLAYER_BPP=16
  
#  -g
#  -DWII_NETTRACE
#  -DTRACK_UNIQUE_MSGIDS  
#  -DUSB_WIILOAD \
#  -DC68K_GEN    
      
CXXFLAGS	=	$(CFLAGS) 

LDFLAGS	=	-g $(MACHDEP) -Wl,-Map,$(notdir $@).map

#---------------------------------------------------------------------------------
# any extra libraries we wish to link with the project
#---------------------------------------------------------------------------------
LIBS	:= \
    -lSDL -lemucommon -ltinysmb -lfat -lwiiuse -lbte -logc -lm -lpng -lfreetype \
    -lz -lwiikeyboard -lbz2

#---------------------------------------------------------------------------------
# list of directories containing libraries, this must be the top level containing
# include and lib
#---------------------------------------------------------------------------------
LIBDIRS	:= \
    wii-emucommon/ \
    wii-emucommon/sdl/SDL/lib \
    wii-emucommon/sdl/SDL_ttf/lib \
    wii-emucommon/sdl/SDL_image/lib

#---------------------------------------------------------------------------------
# no real need to edit anything past this point unless you need to add additional
# rules for different file extensions
#---------------------------------------------------------------------------------
ifneq ($(BUILD),$(notdir $(CURDIR)))
#---------------------------------------------------------------------------------
export OUTPUT	:=	$(CURDIR)/$(TARGET)
export VPATH	:=	$(foreach dir,$(SOURCES),$(CURDIR)/$(dir)) \
					$(foreach dir,$(DATA),$(CURDIR)/$(dir))
export DEPSDIR	:=	$(CURDIR)/$(BUILD)

#---------------------------------------------------------------------------------
# automatically build a list of object files for our project
#---------------------------------------------------------------------------------
CFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c))) 
CPPFILES	:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.cpp)))
sFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.s)))
SFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.S)))
BINFILES	:=	$(foreach dir,$(DATA),$(notdir $(wildcard $(dir)/*.*)))

#---------------------------------------------------------------------------------
# use CXX for linking C++ projects, CC for standard C
#---------------------------------------------------------------------------------
ifeq ($(strip $(CPPFILES)),)
	export LD	:=	$(CC)
else
	export LD	:=	$(CXX)
endif

export OFILES	:=	$(addsuffix .o,$(BINFILES)) \
					$(CPPFILES:.cpp=.o) $(CFILES:.c=.o) \
					$(sFILES:.s=.o) $(SFILES:.S=.o)

#---------------------------------------------------------------------------------
# build a list of include paths
#---------------------------------------------------------------------------------
export INCLUDE	:=	$(foreach dir,$(INCLUDES),-I$(CURDIR)/$(dir)) \
					$(foreach dir,$(LIBDIRS),-I$(dir)/include) \
					-I$(CURDIR)/$(BUILD) \
					-I$(LIBOGC_INC) \
					-I$(DEVKITPRO)/portlibs/ppc/include \
					-I$(DEVKITPRO)/portlibs/ppc/include/freetype2

#---------------------------------------------------------------------------------
# build a list of library paths
#---------------------------------------------------------------------------------
export LIBPATHS	:=	$(foreach dir,$(LIBDIRS),-L$(CURDIR)/$(dir)) \
					-L$(LIBOGC_LIB) \
					-L$(DEVKITPRO)/portlibs/ppc/lib

export OUTPUT	:=	$(CURDIR)/$(TARGET)
.PHONY: $(BUILD) clean

#---------------------------------------------------------------------------------
$(BUILD):
	@[ -d $@ ] || mkdir -p $@
	@make --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile

#---------------------------------------------------------------------------------
clean:
	@echo clean ...
	@rm -fr $(BUILD) $(OUTPUT).elf $(OUTPUT).dol

#---------------------------------------------------------------------------------
run:
	psoload $(TARGET).dol

#---------------------------------------------------------------------------------
reload:
	psoload -r $(TARGET).dol

#---------------------------------------------------------------------------------
else

DEPENDS	:=	$(OFILES:.o=.d)

#---------------------------------------------------------------------------------
# main targets
#---------------------------------------------------------------------------------
$(OUTPUT).dol: $(OUTPUT).elf
$(OUTPUT).elf: $(OFILES)

#---------------------------------------------------------------------------------
# This rule links in binary data with the .jpg extension
#---------------------------------------------------------------------------------
%.jpg.o	:	%.jpg
#---------------------------------------------------------------------------------
	@echo $(notdir $<)
	$(bin2o)

-include $(DEPENDS)

#---------------------------------------------------------------------------------
# This rule links in binary data with the .png extension
#---------------------------------------------------------------------------------
%.png.o	:	%.png
#---------------------------------------------------------------------------------
	@echo $(notdir $<)
	$(bin2o)

-include $(DEPENDS)

%.ttf.o	:	%.ttf
#---------------------------------------------------------------------------------
	@echo $(notdir $<)
	@$(bin2o)

-include $(DEPENDS)

#---------------------------------------------------------------------------------
# This rule links in binary data
#---------------------------------------------------------------------------------
%.bin.o	:	%.bin
#---------------------------------------------------------------------------------
	@echo $(notdir $<)
	@$(bin2o)

%.mod.o	:	%.mod
#---------------------------------------------------------------------------------
	@echo $(notdir $<)
	@$(bin2o)

-include $(DEPENDS)

#---------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------