ifeq ($(strip $(DEVKITARM)),)
$(error Defina DEVKITARM)
endif
ifeq ($(strip $(DEVKITPRO)),)
$(error Defina DEVKITPRO)
endif

TARGET  := hi-entrelinhas
BUILD   := build
SOURCES := source
GFX     := gfx
FONTS   := fonts
AUDIO   := audio

GAME_TITLE     := HELOISA & ISABELA
GAME_SUBTITLE1 := Entrelinhas do Destino
GAME_SUBTITLE2 := Luiz Miguel
GAME_ICON      := icon.bmp

CC      := $(DEVKITARM)/bin/arm-none-eabi-gcc
OBJCOPY := $(DEVKITARM)/bin/arm-none-eabi-objcopy
LIBNDS  := $(DEVKITPRO)/libnds
CALICO  := $(DEVKITPRO)/calico

SPECS := $(CALICO)/share/ds9.specs
ifeq ($(wildcard $(SPECS)),)
$(error Nao achei $(SPECS). Rode: find $(DEVKITPRO) -name \"*.specs\")
endif

ARM7ELF := $(firstword $(shell find $(DEVKITPRO) -type f -name 'ds7_sphynx.elf' 2>/dev/null))
ifeq ($(strip $(ARM7ELF)),)
$(error ds7_sphynx.elf nao encontrado. Rode: find $(DEVKITPRO) -name \"*.elf\")
endif
$(info ARM7 fixado: $(ARM7ELF))
ARM7ARG := -7 $(ARM7ELF)
ARM7_TODOS := $(shell find $(DEVKITPRO) -type f \( -name 'ds7*.elf' -o -name 'default.elf' -o -name '*arm7*.elf' \) 2>/dev/null | sort)

ARCH    := -march=armv5te -mtune=arm946e-s
CFLAGS  := -g -Wall -O2 -fomit-frame-pointer -ffast-math $(ARCH) \
           -specs=$(SPECS) -D__NDS__ -D__ARM9__ -DARM9 \
           -I$(LIBNDS)/include -I$(CALICO)/include -I$(BUILD)
LDFLAGS := -specs=$(SPECS) -g $(ARCH) -Wl,-Map,$(BUILD)/$(TARGET).map
LIBS    := -L$(LIBNDS)/lib -L$(CALICO)/lib -lnds9 -lcalico_ds9 -lcalico_sound9 -lmm9

CFILES     := $(wildcard $(SOURCES)/*.c)
PNGFILES   := $(wildcard $(GFX)/*.png)
FONTFILES  := $(wildcard $(FONTS)/*.NFTR)
AUDIOFILES := $(wildcard $(AUDIO)/*.wav $(AUDIO)/*.mod $(AUDIO)/*.xm $(AUDIO)/*.it $(AUDIO)/*.s3m)
GFXHDRS    := $(patsubst $(GFX)/%.png,$(BUILD)/%.h,$(PNGFILES))
FONTOBJS   := $(patsubst $(FONTS)/%.NFTR,$(BUILD)/%.nftr.o,$(FONTFILES))
OBJS       := $(patsubst $(SOURCES)/%.c,$(BUILD)/%.o,$(CFILES)) \
              $(patsubst $(GFX)/%.png,$(BUILD)/%.o,$(PNGFILES)) \
              $(FONTOBJS) \
              $(BUILD)/soundbank.bin.o

.SECONDARY:
.PHONY: all clean variantes

all: $(TARGET).nds

$(TARGET).nds: $(BUILD)/$(TARGET).elf $(GAME_ICON)
	LC_ALL=C.UTF-8 ndstool -c $@ -9 $< $(ARM7ARG) -b $(GAME_ICON) "$(GAME_TITLE);$(GAME_SUBTITLE1);$(GAME_SUBTITLE2)" -g HIED 00 "ENTRELINHAS"

$(BUILD)/$(TARGET).elf: $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) $(LIBS) -o $@

$(BUILD)/%.o: $(SOURCES)/%.c $(GFXHDRS) $(BUILD)/soundbank.h | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/%.nftr.o: $(FONTS)/%.NFTR | $(BUILD)
	$(OBJCOPY) -I binary -O elf32-littlearm -B arm $< $@

$(BUILD)/soundbank.bin $(BUILD)/soundbank.h: $(AUDIOFILES) | $(BUILD)
	mmutil -d $(AUDIOFILES) -o$(BUILD)/soundbank.bin -h$(BUILD)/soundbank.h

$(BUILD)/soundbank.bin.o: $(BUILD)/soundbank.bin
	$(OBJCOPY) -I binary -O elf32-littlearm -B arm $< $@

$(BUILD)/%.o: $(BUILD)/%.s
	$(CC) $(ARCH) -x assembler-with-cpp -c $< -o $@

$(BUILD)/%.s $(BUILD)/%.h: $(GFX)/%.png | $(BUILD)
	grit $< -gb -gB8 -gT FF00FF -m! -fts -o$(BUILD)/$*

$(BUILD):
	mkdir -p $@

variantes: $(BUILD)/$(TARGET).elf $(GAME_ICON)
	@mkdir -p $(BUILD)/variantes
	@for a in $(ARM7_TODOS); do \
	  n=$$(basename $$a .elf); \
	  echo "ROM de teste com o ARM7 $$n"; \
	  LC_ALL=C.UTF-8 ndstool -c $(BUILD)/variantes/$(TARGET)_$$n.nds -9 $< -7 $$a -b $(GAME_ICON) "$(GAME_TITLE);$(GAME_SUBTITLE1);$(GAME_SUBTITLE2)" -g HIED 00 "ENTRELINHAS" || true; \
	done
	@ls -la $(BUILD)/variantes

clean:
	rm -rf $(BUILD) $(TARGET).nds
