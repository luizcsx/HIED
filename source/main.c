#include <nds.h>

#include "HIED_screen.h"
#include "HIED_screenB.h"

typedef struct {
    const void *bitmap;
    u32         bitmapLen;
    const void *pal;
    u32         palLen;
} Imagem;

#define IMAGEM(nome) { nome##Bitmap, nome##BitmapLen, nome##Pal, nome##PalLen }

static const Imagem TITULO_TOPO  = IMAGEM(HIED_screen);
static const Imagem TITULO_BAIXO = IMAGEM(HIED_screenB);

#define FADE_MIN   (-16)
#define FADE_MAX   0
#define FADE_PASSO 2

int main(void) {
    lcdMainOnTop();

    videoSetMode(MODE_5_2D);
    videoSetModeSub(MODE_5_2D);

    vramSetBankA(VRAM_A_MAIN_BG);
    vramSetBankC(VRAM_C_SUB_BG);

    int bgTopo  = bgInit(3,    BgType_Bmp8, BgSize_B8_256x256, 0, 0);
    int bgBaixo = bgInitSub(3, BgType_Bmp8, BgSize_B8_256x256, 0, 0);

    dmaCopy(TITULO_TOPO.bitmap, bgGetGfxPtr(bgTopo), TITULO_TOPO.bitmapLen);
    dmaCopy(TITULO_TOPO.pal, BG_PALETTE, TITULO_TOPO.palLen);

    dmaCopy(TITULO_BAIXO.bitmap, bgGetGfxPtr(bgBaixo), TITULO_BAIXO.bitmapLen);
    dmaCopy(TITULO_BAIXO.pal, BG_PALETTE_SUB, TITULO_BAIXO.palLen);

    int nivel = FADE_MIN;
    int quadro = 0;
    setBrightness(3, nivel);

    while (1) {
        if (nivel < FADE_MAX) {
            if (++quadro >= FADE_PASSO) {
                quadro = 0;
                nivel++;
                setBrightness(3, nivel);
            }
        }
        swiWaitForVBlank();
    }

    return 0;
}
