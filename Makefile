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
    src/wii/res/fonts \
    src/wii/res/gfx
   
SOURCES		:=	\
    mednafen/src \
    mednafen/src/compress \
    mednafen/src/drivers \
    mednafen/src/sexyal \
    mednafen/src/sexyal/drivers \
    mednafen/src/sound \
    mednafen/src/string \
    mednafen/src/trio \
    mednafen/src/video \
    mednafen/src/gb \
    mednafen/src/gba \
    mednafen/src/lynx \
    mednafen/src/md \
    mednafen/src/md/cart \
    mednafen/src/md/cd \
    mednafen/src/md/input \
    mednafen/src/nes \
    mednafen/src/nes/boards \
    mednafen/src/nes/input \
    mednafen/src/nes/ntsc \
    mednafen/src/nes/ppu \
    mednafen/src/ngp \
    mednafen/src/ngp/TLCS-900h \
    mednafen/src/pce_fast \
    mednafen/src/pcfx \
    mednafen/src/pcfx/input \
    mednafen/src/sms \
    mednafen/src/vb \
    mednafen/src/wswan \
    mednafen/src/hw_cpu/c68k \
    mednafen/src/hw_cpu/v810 \
    mednafen/src/hw_cpu/v810/fpu-new \
    mednafen/src/hw_cpu/z80-fuse \
    mednafen/src/hw_sound/gb_apu \
    mednafen/src/hw_sound/pce_psg \
    mednafen/src/hw_sound/sms_apu \
    mednafen/src/hw_sound/ym2413 \
    mednafen/src/hw_sound/ym2612 \
    mednafen/src/hw_misc/arcade_card \
    mednafen/src/hw_video/huc6270 \
    mednafen/src/mpcdec \
    mednafen/src/cdrom \
    mednafen/src/tremor \
    src/wii \
    src/wii/common \
    src/wii/common/netprint \
    src/wii/common/pngu \
    src/wii/common/i18n \
    src/wii/common/FreeTypeGX \
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
    mednafen/include \
    mednafen/include/blip \
    mednafen/src/drivers \
    mednafen/src/md \
    mednafen/src/ngp \
    mednafen/src/ngp/TLCS-900h \
    mednafen/src/hw_sound \
    mednafen/src/hw_misc \
    mednafen/src/hw_cpu \
    mednafen/src/hw_video \
    src/wii \
    src/wii/common \
    src/wii/common/netprint \
    src/wii/common/pngu \
    src/wii/common/i18n \
    src/wii/common/FreeTypeGX \
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
  -DWII \
  -DVB_BPP=8 \
  -DWSWAN_BPP=16 \
  -DGBA_BPP=16 \
  -DPCE_FAST_BPP=16 \
  -DNES_BPP=16 \
  -DPCFX_BPP=16 \
  -DMD_BPP=16 \
  -DPLAYER_BPP=16
  
#  -DWII_NETTRACE
#  -DTRACK_UNIQUE_MSGIDS  
#  -DUSB_WIILOAD \
#  -DC68K_GEN    
      
CXXFLAGS	=	$(CFLAGS) 

LDFLAGS	=	-g $(MACHDEP) -Wl,-Map,$(notdir $@).map

#---------------------------------------------------------------------------------
# any extra libraries we wish to link with the project
#---------------------------------------------------------------------------------
LIBS	:=	-ltinysmb -lSDL -lfat -lwiiuse -lbte -logc -lm -lpng -lfreetype -lz 

#---------------------------------------------------------------------------------
# list of directories containing libraries, this must be the top level containing
# include and lib
#---------------------------------------------------------------------------------
LIBDIRS	:=

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
					-I$(LIBOGC_INC) -I$(LIBOGC_INC)/sdl

#---------------------------------------------------------------------------------
# build a list of library paths
#---------------------------------------------------------------------------------
export LIBPATHS	:=	$(foreach dir,$(LIBDIRS),-L$(dir)/lib) \
					-L$(LIBOGC_LIB) -I$(LIBOGC_LIB)/sdl


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