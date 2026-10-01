#include <nds.h>

#include "titletop.h"
#include "titlebottom.h"

typedef enum {
    ESTADO_TITULO,
    ESTADO_FADE_OUT,
    ESTADO_FADE_IN
} Estado;

#define FADE_MIN      (-16)  // preto total
#define FADE_MAX      0      // imagem normal
#define FADE_PASSO    2      // quadros por degrau (maior = mais lento)
#define FADE_PAUSA    30     // quadros no preto antes de voltar

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

    dmaCopy(titletopBitmap, bgGetGfxPtr(bgTopo), titletopBitmapLen);
    dmaCopy(titletopPal, BG_PALETTE, titletopPalLen);

    dmaCopy(titlebottomBitmap, bgGetGfxPtr(bgBaixo), titlebottomBitmapLen);
    dmaCopy(titlebottomPal, BG_PALETTE_SUB, titlebottomPalLen);
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
