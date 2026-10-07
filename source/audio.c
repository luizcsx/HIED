#include "audio.h"
#include <maxmod9.h>
#include "soundbank.h"

extern const u8 _binary_build_soundbank_bin_start[];

void audio_iniciar(void)
{
    mmInitDefaultMem((mm_addr)_binary_build_soundbank_bin_start);
}

void audio_tocar_escolha(void)
{
    mmEffect(SFX_NAVIGATION_BUTTON);
}
