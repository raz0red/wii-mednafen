#--param large-function-growth=800 \
#--param inline-unit-growth=200 \

MAKEFLAGS += --no-builtin-rules
MAKE = /usr/bin/make
CFILES := \
	./src/mednafen/src/compress/blz.c \
    ./src/mednafen/src/compress/minilzo.c \
	./src/mednafen/src/compress/unzip.c \
	./src/mednafen/src/compress/ioapi.c \
	./src/mednafen/src/compress/quicklz.c \
	./src/mednafen/src/hw_cpu/v810/fpu-new/softfloat.c \
	./src/mednafen/src/mpcdec/huffsv46.c \
	./src/mednafen/src/mpcdec/huffsv7.c \
	./src/mednafen/src/mpcdec/idtag.c \
	./src/mednafen/src/mpcdec/mpc_decoder.c \
	./src/mednafen/src/mpcdec/mpc_reader.c \
	./src/mednafen/src/mpcdec/requant.c \
	./src/mednafen/src/mpcdec/streaminfo.c \
	./src/mednafen/src/mpcdec/synth_filter.c \
	./src/mednafen/src/string/world_strtod.c \
	./src/mednafen/src/tremor/bitwise.c \
	./src/mednafen/src/tremor/block.c \
	./src/mednafen/src/tremor/codebook.c \
	./src/mednafen/src/tremor/floor0.c \
	./src/mednafen/src/tremor/floor1.c \
	./src/mednafen/src/tremor/framing.c \
	./src/mednafen/src/tremor/info.c \
	./src/mednafen/src/tremor/mapping0.c \
	./src/mednafen/src/tremor/mdct.c \
	./src/mednafen/src/tremor/registry.c \
	./src/mednafen/src/tremor/res012.c \
	./src/mednafen/src/tremor/sharedbook.c \
	./src/mednafen/src/tremor/synthesis.c \
	./src/mednafen/src/tremor/vorbisfile.c \
	./src/mednafen/src/tremor/window.c \
	./src/mednafen/src/trio/trio.c \
	./src/mednafen/src/trio/trionan.c \
	./src/mednafen/src/trio/triostr.c	
	
	
	
CPPFILES := \
	./src/em/em.cpp \
	./src/mednafen/src/pce_fast/hes.cpp \
	./src/mednafen/src/pce_fast/huc.cpp \
	./src/mednafen/src/pce_fast/huc6280.cpp \
	./src/mednafen/src/pce_fast/pce.cpp \
	./src/mednafen/src/pce_fast/pceinput.cpp \
	./src/mednafen/src/pce_fast/tsushin.cpp \
	./src/mednafen/src/pce_fast/vdc.cpp \
	./src/mednafen/src/cdrom/audioreader.cpp \
	./src/mednafen/src/cdrom/cdromfile.cpp \
	./src/mednafen/src/cdrom/cdromif.cpp \
	./src/mednafen/src/cdrom/crc32.cpp \
	./src/mednafen/src/cdrom/galois.cpp \
	./src/mednafen/src/cdrom/l-ec.cpp \
    ./src/mednafen/src/cdrom/lec.cpp \
	./src/mednafen/src/cdrom/pcecd.cpp \
	./src/mednafen/src/cdrom/recover-raw.cpp \
	./src/mednafen/src/cdrom/scsicd.cpp \
	./src/mednafen/src/drivers/ers.cpp \
	./src/mednafen/src/drivers/fps.cpp \
	./src/mednafen/src/drivers/input.cpp \
	./src/mednafen/src/drivers/joystick.cpp \
	./src/mednafen/src/drivers/main.cpp \
	./src/mednafen/src/drivers/sound.cpp \
	./src/mednafen/src/drivers/video.cpp \
	./src/mednafen/src/hw_misc/arcade_card/arcade_card.cpp \
	./src/mednafen/src/hw_sound/pce_psg/pce_psg.cpp	\
	./src/mednafen/src/sexyal/convert.cpp \
	./src/mednafen/src/sexyal/sexyal.cpp \
	./src/mednafen/src/sexyal/drivers/em.cpp \
	./src/mednafen/src/sound/Blip_Buffer.cpp \
	./src/mednafen/src/sound/Fir_Resampler.cpp \
	./src/mednafen/src/string/ConvertUTF.cpp \
	./src/mednafen/src/vb/timer.cpp \
	./src/mednafen/src/vb/vb.cpp \
	./src/mednafen/src/vb/vbinput.cpp \
	./src/mednafen/src/vb/vip.cpp \
	./src/mednafen/src/vb/vsu.cpp \
	./src/mednafen/src/video/font-data.cpp \
	./src/mednafen/src/video/surface.cpp \
	./src/mednafen/src/video/text.cpp \
	./src/mednafen/src/video/video_.cpp \
	./src/mednafen/src/endian.cpp \
	./src/mednafen/src/error.cpp \
	./src/mednafen/src/file.cpp \
	./src/mednafen/src/general.cpp \
	./src/mednafen/src/math_ops.cpp	\
	./src/mednafen/src/md5.cpp \
	./src/mednafen/src/mednafen.cpp \
	./src/mednafen/src/memory.cpp \
	./src/mednafen/src/mempatcher.cpp \
	./src/mednafen/src/okiadpcm.cpp \
	./src/mednafen/src/player.cpp \
	./src/mednafen/src/settings.cpp \
	./src/mednafen/src/state.cpp \
	./src/mednafen/src/tests.cpp			

	

FILES := $(patsubst %.c,%.o,$(CFILES)) $(patsubst %.cpp,%.o,$(CPPFILES))

FLAGS := \
	-O3 \
    $(CFLAGS) \
	-I. \
	-I./src/mednafen/ \
	-I./src/mednafen/include/ \
	-I./src/mednafen/src/drivers/ \
	-I./src/mednafen/src/pce_fast/ \
	-I./src/mednafen/src/hw_misc/ \
	-I./src/mednafen/src/hw_sound/ \
    -DWRC -DWII \
 	-DHAVE_CONFIG_H -DHAVE_FLOAT_H \
  	-Winline \
  	-fomit-frame-pointer \
	-fno-strict-overflow \
	-fsigned-char \
  	-Wno-strict-aliasing \
  	-Wno-narrowing \
  	-DWSWAN_BPP=16 \
  	-DGBA_BPP=16 \
  	-DNES_BPP=16 \
  	-DPCFX_BPP=16 \
  	-DMD_BPP=16 \
  	-DPLAYER_BPP=16 \
    -flto

LINK_FLAGS := \
    -O3 \
	-lz \
    -s MODULARIZE=1 \
    -s EXPORT_NAME="'mednafen'" \
    -s TOTAL_MEMORY=67108864 \
    -s ALLOW_MEMORY_GROWTH=1 \
	-s ASSERTIONS=0 \
	-s EXIT_RUNTIME=0 \
	-s EXPORTED_RUNTIME_METHODS="['FS', 'cwrap']" \
    -s EXPORTED_FUNCTIONS="['_LoadGame', '_emInit', '_emStep', '_emPceSet6PadEnabled']" \
	-s INVOKE_RUN=0 \
    -flto

all: mednafen

mednafen:
	@echo mednafen
	$(MAKE) mednafen.js

mednafen.js: $(FILES)
	emcc -o $@ $(FILES) $(LINK_FLAGS) 	

%.o : %.cpp
	emcc -c $< -o $@ \
	$(FLAGS)

%.o : %.c
	emcc -c $< -o $@ \
	$(FLAGS)

clean:
	@echo "Cleaning"
	@echo $(FILES)
	rm -fr *.o */*.o */*/*.o */*/*/*.o */*/*/*/*.o	
