#include "audio.h"
#include <maxmod9.h>

extern const u8 _binary_build_soundbank_bin_start[];

void audio_iniciar(void)
{
    mmInitDefaultMem((mm_addr)_binary_build_soundbank_bin_start);
}
