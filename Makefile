ifeq ($(strip $(DEVKITARM)),)
$(error Defina DEVKITARM, por exemplo: export DEVKITARM=/opt/devkitpro/devkitARM)
endif
ifeq ($(strip $(DEVKITPRO)),)
$(error Defina DEVKITPRO, por exemplo: export DEVKITPRO=/opt/devkitpro)
endif

TARGET  := hi-entrelinhas
BUILD   := build
SOURCES := source
GFX     := gfx

GAME_TITLE     := Heloísa & Isabela
GAME_SUBTITLE1 := Entrelinhas do Destino
GAME_SUBTITLE2 := Luiz Miguel
# GAME_ICON      := $(DEVKITPRO)/libnds/icon.bmp

CC     := $(DEVKITARM)/bin/arm-none-eabi-gcc
LIBNDS := $(DEVKITPRO)/libnds
CALICO := $(DEVKITPRO)/calico

ARCH    := -mthumb -mthumb-interwork
CFLAGS  := -g -Wall -O2 -march=armv5te -mtune=arm946e-s -fomit-frame-pointer \
           -ffast-math $(ARCH) -specs=ds_arm9.specs \
           -D__NDS__ -D__ARM9__ -DARM9 \
           -I$(LIBNDS)/include -I$(CALICO)/include -I$(BUILD)
LDFLAGS := -specs=ds_arm9.specs -g $(ARCH) -Wl,-Map,$(BUILD)/$(TARGET).map
LIBS    := -L$(LIBNDS)/lib -L$(CALICO)/lib -lnds9 -lcalico_ds9

CFILES   := $(wildcard $(SOURCES)/*.c)
PNGFILES := $(wildcard $(GFX)/*.png)
GFXHDRS  := $(patsubst $(GFX)/%.png,$(BUILD)/%.h,$(PNGFILES))
OBJS     := $(patsubst $(SOURCES)/%.c,$(BUILD)/%.o,$(CFILES)) \
            $(patsubst $(GFX)/%.png,$(BUILD)/%.o,$(PNGFILES))

.SECONDARY:
.PHONY: all clean

all: $(TARGET).nds

$(TARGET).nds: $(BUILD)/$(TARGET).elf
	ndstool -c $@ -9 $< -b $(GAME_ICON) "$(GAME_TITLE);$(GAME_SUBTITLE1);$(GAME_SUBTITLE2)"

$(BUILD)/$(TARGET).elf: $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) $(LIBS) -o $@

$(BUILD)/%.o: $(SOURCES)/%.c $(GFXHDRS) | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/%.o: $(BUILD)/%.s
	$(CC) $(ARCH) -x assembler-with-cpp -c $< -o $@

# gfx/<nome>.png -> build/<nome>.s + build/<nome>.h (símbolos: <nome>Bitmap, <nome>Pal, ...Len)
$(BUILD)/%.s $(BUILD)/%.h: $(GFX)/%.png | $(BUILD)
	grit $< -gb -gB8 -m! -fts -o$(BUILD)/$*

$(BUILD):
	mkdir -p $@

clean:
	rm -rf $(BUILD) $(TARGET).nds
