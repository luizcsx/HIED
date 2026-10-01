#include <nds.h>
#include <stdbool.h>

#ifndef SEM_BOOT
#include "Boot_screen.h"
#include "Boot_screenB.h"
#endif
#include "HIED_screen.h"
#include "HIED_screenB.h"

typedef struct {
    const void *bitmap;
    u32         bitmapLen;
    const void *pal;
    u32         palLen;
} Imagem;

#define IMAGEM(nome) { nome##Bitmap, nome##BitmapLen, nome##Pal, nome##PalLen }

#ifndef SEM_BOOT
static const Imagem BOOT_TOPO    = IMAGEM(Boot_screen);
static const Imagem BOOT_BAIXO   = IMAGEM(Boot_screenB);
#endif
static const Imagem TITULO_TOPO  = IMAGEM(HIED_screen);
static const Imagem TITULO_BAIXO = IMAGEM(HIED_screenB);

typedef enum {
    ESTADO_BOOT_ENTRA,
    ESTADO_BOOT_ESPERA,
    ESTADO_BOOT_SAI,
    ESTADO_TITULO_ENTRA,
    ESTADO_TITULO_ESPERA,
    ESTADO_TITULO_SAI,
    ESTADO_TITULO_PAUSA
} Estado;

#define FADE_MIN      (-16)
#define FADE_MAX      0
#define FADE_PASSO    2
#define FADE_PAUSA    30
#define BOOT_DURACAO  180

#define TECLAS_AVANCAR (KEY_TOUCH | KEY_A | KEY_START)

static int bgTopo;
static int bgBaixo;
static int nivel       = FADE_MIN;
static int quadro_fade = 0;

static void iniciar_video(void)
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

static void mostrar_telas(const Imagem *topo, const Imagem *baixo)
{
    dmaCopy(topo->bitmap, bgGetGfxPtr(bgTopo), topo->bitmapLen);
    dmaCopy(topo->pal, BG_PALETTE, topo->palLen);

    dmaCopy(baixo->bitmap, bgGetGfxPtr(bgBaixo), baixo->bitmapLen);
    dmaCopy(baixo->pal, BG_PALETTE_SUB, baixo->palLen);
}

static bool fade_para(int alvo)
{
    if (++quadro_fade >= FADE_PASSO) {
        quadro_fade = 0;
        if (nivel < alvo) nivel++;
        else if (nivel > alvo) nivel--;
        setBrightness(3, nivel);
    }
    return nivel == alvo;
}

int main(void)
{
    iniciar_video();

#ifdef SEM_BOOT
    mostrar_telas(&TITULO_TOPO, &TITULO_BAIXO);
    Estado estado = ESTADO_TITULO_ENTRA;
#else
    mostrar_telas(&BOOT_TOPO, &BOOT_BAIXO);
    Estado estado = ESTADO_BOOT_ENTRA;
#endif
    int espera = 0;

    while (1) {
        scanKeys();
        u32 apertou = keysDown();

        switch (estado) {
        case ESTADO_BOOT_ENTRA:
            if (apertou & TECLAS_AVANCAR) {
                estado = ESTADO_BOOT_SAI;
            } else if (fade_para(FADE_MAX)) {
                estado = ESTADO_BOOT_ESPERA;
                espera = BOOT_DURACAO;
            }
            break;

        case ESTADO_BOOT_ESPERA:
            if ((apertou & TECLAS_AVANCAR) || --espera <= 0) {
                estado = ESTADO_BOOT_SAI;
            }
            break;

        case ESTADO_BOOT_SAI:
            if (fade_para(FADE_MIN)) {
                mostrar_telas(&TITULO_TOPO, &TITULO_BAIXO);
                estado = ESTADO_TITULO_ENTRA;
            }
            break;

        case ESTADO_TITULO_ENTRA:
            if (fade_para(FADE_MAX)) {
                estado = ESTADO_TITULO_ESPERA;
            }
            break;

        case ESTADO_TITULO_ESPERA:
            if (apertou & TECLAS_AVANCAR) {
                estado = ESTADO_TITULO_SAI;
            }
            break;

        case ESTADO_TITULO_SAI:
            if (fade_para(FADE_MIN)) {
                estado = ESTADO_TITULO_PAUSA;
                espera = FADE_PAUSA;
            }
            break;

        case ESTADO_TITULO_PAUSA:
            if (--espera <= 0) {
                estado = ESTADO_TITULO_ENTRA;
            }
            break;
        }

        swiWaitForVBlank();
    }

    return 0;
}
