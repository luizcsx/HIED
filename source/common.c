#include "comum.h"

int bgTopo;
int bgBaixo;
int nivel       = FADE_MIN;
static int quadro_fade = 0;

void iniciar_video(void)
{
    lcdMainOnTop();

    videoSetMode(MODE_5_2D);
    videoSetModeSub(MODE_5_2D);

    vramSetBankA(VRAM_A_MAIN_BG);
    vramSetBankC(VRAM_C_SUB_BG);

    bgTopo  = bgInit(3,    BgType_Bmp8, BgSize_B8_256x256, 0, 0);
    bgBaixo = bgInitSub(3, BgType_Bmp8, BgSize_B8_256x256, 0, 0);

    setBrightness(3, nivel);
}

void mostrar_telas(const Imagem *topo, const Imagem *baixo)
{
    dmaCopy(topo->bitmap, bgGetGfxPtr(bgTopo), topo->bitmapLen);
    dmaCopy(topo->pal, BG_PALETTE, topo->palLen);

    dmaCopy(baixo->bitmap, bgGetGfxPtr(bgBaixo), baixo->bitmapLen);
    dmaCopy(baixo->pal, BG_PALETTE_SUB, baixo->palLen);
}

bool fade_para(int alvo)
{
    if (++quadro_fade >= FADE_PASSO) {
        quadro_fade = 0;
        if (nivel < alvo) nivel++;
        else if (nivel > alvo) nivel--;
        setBrightness(3, nivel);
    }
    return nivel == alvo;
}
