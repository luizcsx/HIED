#include <nds.h>

#include "HIED_screen.h"
#include "HIED_screenB.h"

typedef enum {
    ESTADO_TITULO,
    ESTADO_FADE_OUT,
    ESTADO_FADE_IN
} Estado;

#define FADE_MIN      (-16) 
#define FADE_MAX      0
#define FADE_PASSO    2
#define FADE_PAUSA    30

static int bgTopo;
static int bgBaixo;

static void iniciar_video(void)
{
    lcdMainOnTop();

    videoSetMode(MODE_5_2D);
    videoSetModeSub(MODE_5_2D);

    vramSetBankA(VRAM_A_MAIN_BG);
    vramSetBankC(VRAM_C_SUB_BG);

    bgTopo  = bgInit(3,    BgType_Bmp8, BgSize_B8_256x256, 0, 0);
    bgBaixo = bgInitSub(3, BgType_Bmp8, BgSize_B8_256x256, 0, 0);

    dmaCopy(HIED_screenBitmap,  bgGetGfxPtr(bgTopo), HIED_screenBitmapLen);
    dmaCopy(HIED_screenPal,     BG_PALETTE,          HIED_screenPalLen);
    
    dmaCopy(HIED_screenBBitmap, bgGetGfxPtr(bgBaixo), HIED_screenBBitmapLen);
    dmaCopy(HIED_screenBPal,    BG_PALETTE_SUB,       HIED_screenBPalLen);
}

int main(void)
{
    iniciar_video();

    Estado estado = ESTADO_TITULO;
    int nivel  = FADE_MAX;
    int quadro = 0;
    int pausa  = 0;

    setBrightness(3, nivel);

    while (1) {
        scanKeys();
        u32 apertou = keysDown();

        switch (estado) {
        case ESTADO_TITULO:
            if (apertou & (KEY_TOUCH | KEY_A | KEY_START)) {
                estado = ESTADO_FADE_OUT;
                quadro = 0;
            }
            break;

        case ESTADO_FADE_OUT:
            if (++quadro % FADE_PASSO == 0) {
                nivel--;
                setBrightness(3, nivel);
                if (nivel <= FADE_MIN) {
                    estado = ESTADO_FADE_IN;
                    pausa  = FADE_PAUSA;
                    quadro = 0;
                }
            }
            break;

        case ESTADO_FADE_IN:
            if (pausa > 0) {
                pausa--;
            } else if (++quadro % FADE_PASSO == 0) {
                nivel++;
                setBrightness(3, nivel);
                if (nivel >= FADE_MAX) {
                    estado = ESTADO_TITULO;
                }
            }
            break;
        }

        swiWaitForVBlank();
    }

    return 0;
}
