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

    setBrightness(3, 0);

    while (1) {
        swiWaitForVBlank();
    }

    return 0;
}
