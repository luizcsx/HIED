#include "audio.h"

extern const s16 navigation_button_pcm[];
extern const s16 navigation_button_pcm_end[];

static bool pronto = false;

void audio_iniciar(void)
{
    pronto = false;
}

void audio_tocar_escolha(void)
{
    if (!pronto) {
        return;
    }

    int bytes = (const u8*)navigation_button_pcm_end - (const u8*)navigation_button_pcm;

    soundPlaySample(
        (const void*)navigation_button_pcm,
        SoundFormat_16Bit,
        bytes,
        48000,
        127,
        64,
        false,
        0
    );
}
