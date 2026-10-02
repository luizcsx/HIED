#include <nds.h>

int main(void) {
    videoSetMode(MODE_5_2D);
    videoSetModeSub(MODE_5_2D);

    vramSetBankA(VRAM_A_MAIN_BG);
    vramSetBankC(VRAM_C_SUB_BG);

    int bgTopo  = bgInit(3,    BgType_Bmp8, BgSize_B8_256x256, 0, 0);
    int bgBaixo = bgInitSub(3, BgType_Bmp8, BgSize_B8_256x256, 0, 0);

    u16 *paletaTopo  = BG_PALETTE;
    u16 *paletaBaixo = BG_PALETTE_SUB;
    paletaTopo[1]  = RGB15(31, 0, 0);
    paletaBaixo[1] = RGB15(0, 0, 31);

    u8 *gfxTopo  = (u8*)bgGetGfxPtr(bgTopo);
    u8 *gfxBaixo = (u8*)bgGetGfxPtr(bgBaixo);
    for (int i = 0; i < 256 * 192; i++) {
        gfxTopo[i]  = 1;
        gfxBaixo[i] = 1;
    }

    setBrightness(3, 0);

    while (1) {
        swiWaitForVBlank();
    }

    return 0;
}
