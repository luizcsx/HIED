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
GAME_ICON      := icon.bmp

CC     := $(DEVKITARM)/bin/arm-none-eabi-gcc
LIBNDS := $(DEVKITPRO)/libnds
CALICO := $(DEVKITPRO)/calico

SPECS := $(CALICO)/share/ds9.specs
ifeq ($(wildcard $(SPECS)),)
$(error Nao achei $(SPECS). Rode: find $(DEVKITPRO) -name "*.specs")
endif

ARM7ELF := $(firstword \
    $(wildcard $(CALICO)/bin/ds7_maxmod.elf $(CALICO)/bin/ds7_nomaxmod.elf $(LIBNDS)/default.elf) \
    $(shell find $(DEVKITPRO) -type f \( -name 'ds7*.elf' -o -name 'default.elf' -o -name '*arm7*.elf' \) 2>/dev/null | sort))
ifeq ($(strip $(ARM7ELF)),)
$(error Nenhum ARM7 padrao encontrado. Instale o pacote do ARM7 (provavelmente: dkp-pacman -S default-arm7) ou rode: find $(DEVKITPRO) -name "*.elf")
endif
$(info ARM7 padrao: $(ARM7ELF))
ARM7ARG := -7 $(ARM7ELF)

ARCH    := -march=armv5te -mtune=arm946e-s
CFLAGS  := -g -Wall -O2 -fomit-frame-pointer -ffast-math $(ARCH) \
           -specs=$(SPECS) -D__NDS__ -D__ARM9__ -DARM9 \
           -I$(LIBNDS)/include -I$(CALICO)/include -I$(BUILD)
LDFLAGS := -specs=$(SPECS) -g $(ARCH) -Wl,-Map,$(BUILD)/$(TARGET).map
LIBS    := -L$(LIBNDS)/lib -L$(CALICO)/lib -lnds9 -lcalico_ds9

CFILES   := $(wildcard $(SOURCES)/*.c)
PNGFILES := $(wildcard $(GFX)/*.png)
GFXHDRS  := $(patsubst $(GFX)/%.png,$(BUILD)/%.h,$(PNGFILES))
OBJS     := $(patsubst $(SOURCES)/%.c,$(BUILD)/%.o,$(CFILES)) \
            $(patsubst $(GFX)/%.png,$(BUILD)/%.o,$(PNGFILES))

.SECONDARY:
.PHONY: all clean

all: $(TARGET).nds

$(TARGET).nds: $(BUILD)/$(TARGET).elf $(GAME_ICON)
	ndstool -c $@ -9 $< $(ARM7ARG) -b $(GAME_ICON) "$(GAME_TITLE);$(GAME_SUBTITLE1);$(GAME_SUBTITLE2)"

$(BUILD)/$(TARGET).elf: $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) $(LIBS) -o $@

$(BUILD)/%.o: $(SOURCES)/%.c $(GFXHDRS) | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/%.o: $(BUILD)/%.s
	$(CC) $(ARCH) -x assembler-with-cpp -c $< -o $@

$(BUILD)/%.s $(BUILD)/%.h: $(GFX)/%.png | $(BUILD)
	grit $< -gb -gB8 -m! -fts -o$(BUILD)/$*

$(BUILD):
	mkdir -p $@

clean:
	rm -rf $(BUILD) $(TARGET).nds
