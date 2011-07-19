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
    mednafen/src/lynx \
    mednafen/src/vb \
    mednafen/src/wswan \
    mednafen/src/hw_cpu/v810 \
    mednafen/src/hw_cpu/v810/fpu-new \
    mednafen/src/hw_sound/gb_apu \
    src/wii \
    src/wii/common \
    src/wii/common/netprint \
    src/wii/common/pngu \
    src/wii/common/i18n \
    src/wii/common/FreeTypeGX \
    src/wii/emulator \
    src/wii/emulator/helper \
    src/wii/emulator/gb \
    src/wii/emulator/lynx \
    src/wii/emulator/vb \
    src/wii/emulator/wswan
    
DATA		:=	src/wii/res/fonts src/wii/res/gfx  
INCLUDES	:= \
    mednafen/include \
    mednafen/include/blip \
    mednafen/src/drivers \
    mednafen/src/hw_sound \
    src/wii \
    src/wii/common \
    src/wii/common/netprint \
    src/wii/common/pngu \
    src/wii/common/i18n \
    src/wii/common/FreeTypeGX \
    src/wii/emulator \
    src/wii/emulator/helper \
    src/wii/emulator/gb \
    src/wii/emulator/lynx \
    src/wii/emulator/vb \
    src/wii/emulator/wswan

#---------------------------------------------------------------------------------
# options for code generation
#---------------------------------------------------------------------------------  
#CFLAGS  -Wall -DWII_NETTRACE -funsafe-math-optimizations -DWII_NETTRACE -DTRACK_UNIQUE_MSGIDS
CFLAGS	= \
  -g -O3 $(MACHDEP) $(INCLUDE) -DHAVE_CONFIG_H -DHAVE_FLOAT_H -funsafe-math-optimizations \
  -DWII \
  -DVB_BPP=8 \
  -DWSWAN_BPP=16
#  -DWII_NETTRACE \
#  -DUSB_WIILOAD 
      
CXXFLAGS	=	$(CFLAGS) 

LDFLAGS	=	-g $(MACHDEP) -Wl,-Map,$(notdir $@).map

#---------------------------------------------------------------------------------
# any extra libraries we wish to link with the project
#---------------------------------------------------------------------------------
LIBS	:=	-lSDL_ttf -lSDL -lSDL_image -lfat -lwiiuse -lbte -logc -lm -lpng -lfreetype -lz

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
