#include "audio.h"
#include <maxmod9.h>
#include "soundbank.h"

extern const u8 _binary_build_soundbank_bin_start[];

static bool pronto = false;

void audio_iniciar(void)
{
    mmInitDefaultMem((mm_addr)_binary_build_soundbank_bin_start);
    mmLoadEffect(SFX_NAVIGATION_BUTTON);
    pronto = true;
}

void audio_tocar_escolha(void)
{
    if (pronto) {
        mmEffect(SFX_NAVIGATION_BUTTON);
    }
}
