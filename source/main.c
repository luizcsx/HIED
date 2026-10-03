#include <nds.h>
#include <stdbool.h>
#include <string.h>

#include "HIED_screen.h"
#include "HIED_screenB.h"
#include "SS_screenbg.h"
#include "SS_screenbgB.h"
#include "Button_template.h"

typedef struct {
    const void *bitmap;
    u32         bitmapLen;
    const void *pal;
    u32         palLen;
} Imagem;

#define IMAGEM(nome) { nome##Bitmap, nome##BitmapLen, nome##Pal, nome##PalLen }

static const Imagem TITULO_TOPO   = IMAGEM(HIED_screen);
static const Imagem TITULO_BAIXO  = IMAGEM(HIED_screenB);
static const Imagem SELECAO_TOPO  = IMAGEM(SS_screen);
static const Imagem SELECAO_BAIXO = IMAGEM(SS_screenB);

typedef enum {
    ESTADO_TITULO_ENTRA,
    ESTADO_TITULO_ESPERA,
    ESTADO_TITULO_SAI,

    ESTADO_SELECAO_ENTRA,
    ESTADO_SELECAO_ESPERA
} Estado;

#define FADE_MIN   (-16)
#define FADE_MAX   0
#define FADE_PASSO 2

#define TECLAS_AVANCAR  (KEY_A | KEY_START)
#define TECLAS_ESQUERDA (KEY_LEFT | KEY_UP)
#define TECLAS_DIREITA  (KEY_RIGHT | KEY_DOWN)

#define COR_CONTORNO 1

static int bgTopo;
static int bgBaixo;
static int nivel       = FADE_MIN;
static int quadro_fade = 0;

static u8 telaBaixoBuf[256 * 192];

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

static bool tocou(void)
{
    touchPosition toque;
    touchRead(&toque);
    return (keysHeld() & KEY_TOUCH) && toque.px != 0 && toque.py != 0;
}

typedef struct { int x, y; } Ponto;

static const Ponto SLOTS[3] = {
    { 44,  60 },
    { 128, 118 },
    { 212, 60 }
};

#define SLOT_RAIO 32

static int slot_tocado(void)
{
    touchPosition toque;
    touchRead(&toque);
    if (!(keysHeld() & KEY_TOUCH)) return -1;

    for (int i = 0; i < 3; i++) {
        int dx = toque.px - SLOTS[i].x;
        int dy = toque.py - SLOTS[i].y;
        if (dx * dx + dy * dy <= SLOT_RAIO * SLOT_RAIO) return i;
    }
    return -1;
}

static inline void plotar(int x, int y, u8 indice)
{
    if ((unsigned)x < 256 && (unsigned)y < 192) {
        telaBaixoBuf[y * 256 + x] = indice;
    }
}

static void desenhar_anel(int cx, int cy, int raio, u8 indice, int espessura)
{
    int r2max = raio * raio;
    int r2min = (raio - espessura) * (raio - espessura);
    for (int y = -raio; y <= raio; y++) {
        for (int x = -raio; x <= raio; x++) {
            int d2 = x * x + y * y;
            if (d2 <= r2max && d2 >= r2min) {
                plotar(cx + x, cy + y, indice);
            }
        }
    }
}

static void desenhar_botao(int cx, int cy)
{
    const u8 *src = (const u8 *)ButtonBitmap;
    for (int y = 0; y < 64; y++) {
        for (int x = 0; x < 64; x++) {
            u8 indice = src[y * 64 + x];
            if (indice != 0) {
                plotar(cx - 32 + x, cy - 32 + y, indice);
            }
        }
    }
}

static void montar_tela_selecao(int slot_atual)
{
    memcpy(telaBaixoBuf, SS_screenBBitmap, sizeof(telaBaixoBuf));

    for (int i = 0; i < 3; i++) {
        desenhar_botao(SLOTS[i].x, SLOTS[i].y);
        if (i == slot_atual) {
            desenhar_anel(SLOTS[i].x, SLOTS[i].y, 34, COR_CONTORNO, 2);
        }
    }

    dmaCopy(telaBaixoBuf, bgGetGfxPtr(bgBaixo), sizeof(telaBaixoBuf));
}

int main(void)
{
    iniciar_video();
    mostrar_telas(&TITULO_TOPO, &TITULO_BAIXO);

    Estado estado = ESTADO_TITULO_ENTRA;
    int slot_selecionado = 0;

    scanKeys();
    scanKeys();

    while (1) {
        swiWaitForVBlank();
        scanKeys();
        u32 apertou = keysDown();

        switch (estado) {
        case ESTADO_TITULO_ENTRA:
            if (fade_para(FADE_MAX)) {
                estado = ESTADO_TITULO_ESPERA;
            }
            break;

        case ESTADO_TITULO_ESPERA:
            if ((apertou & TECLAS_AVANCAR) || tocou()) {
                estado = ESTADO_TITULO_SAI;
            }
            break;

        case ESTADO_TITULO_SAI:
            if (fade_para(FADE_MIN)) {
                dmaCopy(SELECAO_TOPO.bitmap, bgGetGfxPtr(bgTopo), SELECAO_TOPO.bitmapLen);
                dmaCopy(SELECAO_TOPO.pal, BG_PALETTE, SELECAO_TOPO.palLen);
                dmaCopy(SELECAO_BAIXO.pal, BG_PALETTE_SUB, SELECAO_BAIXO.palLen);
                montar_tela_selecao(slot_selecionado);
                estado = ESTADO_SELECAO_ENTRA;
            }
            break;

        case ESTADO_SELECAO_ENTRA:
            if (fade_para(FADE_MAX)) {
                estado = ESTADO_SELECAO_ESPERA;
            }
            break;

        case ESTADO_SELECAO_ESPERA: {
            int toque_slot = slot_tocado();
            bool mudou = false;

            if (apertou & TECLAS_ESQUERDA) {
                slot_selecionado = (slot_selecionado + 2) % 3;
                mudou = true;
            } else if (apertou & TECLAS_DIREITA) {
                slot_selecionado = (slot_selecionado + 1) % 3;
                mudou = true;
            } else if (toque_slot >= 0 && toque_slot != slot_selecionado) {
                slot_selecionado = toque_slot;
                mudou = true;
            }

            if (mudou) {
                montar_tela_selecao(slot_selecionado);
            }
            break;
        }
        }
    }

    return 0;
}
