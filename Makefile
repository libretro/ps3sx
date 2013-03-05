
# CELL_GPU_TYPE (currently RSX is only one option)  
CELL_GPU_TYPE = RSX    
#CELL_PSGL_VERSION is debug, dpm or opt  
CELL_PSGL_VERSION = opt  

CELL_SDK ?= /usr/local/cell
CELL_MK_DIR ?= $(CELL_SDK)/samples/mk
include $(CELL_MK_DIR)/sdk.makedef.mk

MKFSELF_NPDRM = $(CELL_SDK)/host-win32/bin/make_fself_npdrm
MKPKG_NPDRM = $(CELL_SDK)/host-win32/bin/make_package_npdrm

PPU_OPTIMIZE_LV = -O2

PPU_SRCS		:= Ps3sxMain.cpp ./ps3/ps3audio.cpp ./ps3/ps3video.cpp ./ps3/rom_list.cpp \
	./ps3/input/cellInput.cpp ./ps3/graphics/PSGLGraphics.cpp ./ps3/fileio/FileBrowser.cpp\
	./ps3/buffer.c ./ps3/resampler.c ./ps3/ini/ini.c ./ps3/ini/iniFile.cpp \
	./pcsxcore/psxbios.c	\
	./pcsxcore/cdrom.c	\
	./pcsxcore/psxcounters.c	\
	./pcsxcore/psxdma.c	\
	./pcsxcore/disr3000a.c	\
	./pcsxcore/spu.c	\
	./pcsxcore/sio.c	\
	./pcsxcore/psxhw.c	\
	./pcsxcore/mdec.c	\
	./pcsxcore/psxmem.c	\
	./pcsxcore/misc.c	\
	./pcsxcore/GPU_SOFT/cfg.c \
	./pcsxcore/GPU_SOFT/Gudraw.c \
	./pcsxcore/GPU_SOFT/fps.c \
	./pcsxcore/GPU_SOFT/gpu.c \
	./pcsxcore/GPU_SOFT/menu.c \
	./pcsxcore/GPU_SOFT/prim.c \
	./pcsxcore/GPU_SOFT/soft.c \
	./pcsxcore/GPU_SOFT/zn.c \
	./pcsxcore/plugins.c	\
	./pcsxcore/plugin.c	\
	./pcsxcore/decode_xa.c	\
	./pcsxcore/ppc/pasm.s	\
	./pcsxcore/r3000a.c	\
	./pcsxcore/psxinterpreter.c	\
	./pcsxcore/gte.c	\
	./pcsxcore/psxhle.c	\
	./pcsxcore/ppc/pR3000A.c	\
	./pcsxcore/ppc/ppc.c	\
	./pcsxcore/ppc/reguse.c \
	./pcsxcore/PlugPAD.c \
	./pcsxcore/PlugCD.c \
	./pcsxcore/dfsound/adsr.c \
	./pcsxcore/dfsound/xa.c \
	./pcsxcore/dfsound/spu.c \
	./pcsxcore/dfsound/dma.c \
	./pcsxcore/dfsound/registers.c \
	./pcsxcore/dfsound/freeze.c \
	./pcsxcore/zlib/adler32.c \
	./pcsxcore/zlib/compress.c \
	./pcsxcore/zlib/crc32.c \
	./pcsxcore/zlib/deflate.c \
	./pcsxcore/zlib/gzio.c \
	./pcsxcore/zlib/infblock.c \
	./pcsxcore/zlib/infcodes.c \
	./pcsxcore/zlib/inffast.c \
	./pcsxcore/zlib/inflate.c \
	./pcsxcore/zlib/inftrees.c \
	./pcsxcore/zlib/infutil.c \
	./pcsxcore/zlib/trees.c \
	./pcsxcore/zlib/uncompr.c \
	./pcsxcore/zlib/zutil.c 

PPU_TARGET		=	ps3sx.elf

PPU_CPPFLAGS		+= -D__BIGENDIAN__ -D__ppc__ -D_BIG_ENDIAN -DBIG_ENDIAN -D'PACKAGE_VERSION="0.3"' -DPS3_SDK_3_41 -DPSGL

PPU_ASFLAGS		+=

PPU_LDFLAGS		= -Wl -finline-limit=5000

PPU_CXXFLAGS		+=	-I. -I./ps3 -I./pcsxcore -I./pcsxcore/zlib -I./pcsxcore/ppc -I./ps3/threads -I./ps3/Audio -I./ps3/kammysource/libkammy/include -Ips3/input -Ips3/ini
PPU_LIBS		    +=	$(CELL_TARGET_PATH)/ppu/lib/libgcm_cmd.a \
					$(CELL_TARGET_PATH)/ppu/lib/libgcm_sys_stub.a $(CELL_TARGET_PATH)/ppu/lib/libfs_stub.a
PPU_LDLIBS		+= 	-L$(CELL_SDK)/target/ppu/lib/PSGL/RSX/opt -lPSGL -lPSGLcgc -lcgc -lsysmodule_stub  -lresc_stub -lm -ldbgfont -lsysutil_stub -lio_stub -laudio_stub -lpthread ./ps3/kammysource/libkammy/libkammy.a

include $(CELL_MK_DIR)/sdk.target.mk

$(VPSHADER_PPU_OBJS): $(OBJS_DIR)/%.ppu.o : %.vpo
	@mkdir -p $(dir $(@))
	$(PPU_OBJCOPY)  -I binary -O elf64-powerpc-celloslv2 -B powerpc $< $@

$(FPSHADER_PPU_OBJS): $(OBJS_DIR)/%.ppu.o : %.fpo
	@mkdir -p $(dir $(@))
	$(PPU_OBJCOPY)  -I binary -O elf64-powerpc-celloslv2 -B powerpc $< $@
