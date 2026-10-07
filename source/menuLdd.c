#include "menuLdd.h"
#include "salvar.h"
#include "SS_screenbg.h"
#include "SS_screenbgB.h"
#include "Button_template.h"
#include "SS_screenbg_selXbtn.h"
#include "SS_screenbg_selYbtn.h"

extern const u8 _binary_fonts_heloFont_NFTR_start[];

static const u8 *const FONTE = _binary_fonts_heloFont_NFTR_start;

static const Imagem SELECAO_TOPO    = IMAGEM(SS_screenbg);
static const Imagem SELECAO_BAIXO   = IMAGEM(SS_screenbgB);
static const Imagem SELECAO_EXCLUIR = IMAGEM(SS_screenbg_selXbtn);
static const Imagem SELECAO_DUPLICAR = IMAGEM(SS_screenbg_selYbtn);

typedef enum {
    MENU_ENTRA,
    MENU_ESPERA,
    MENU_SAI
} MenuEstado;

typedef enum {
    DESTINO_TITULO,
    DESTINO_SELECAO
} Destino;

typedef enum {
    MODO_NORMAL,
    MODO_EXCLUIR,
    MODO_DUPLICAR
} Modo;

#define TECLAS_ESQUERDA (KEY_LEFT | KEY_UP)
#define TECLAS_DIREITA  (KEY_RIGHT | KEY_DOWN)

#define COR_CONTORNO 255
#define COR_CONTORNO_RGB RGB15(22, 16, 6)
#define COR_CONTORNO_EXCLUIR_RGB RGB15(15, 5, 5)
#define COR_CONTORNO_DUPLICAR_RGB RGB15(19, 0, 31)
#define RODAPE_DESCIDA 40
#define RODAPE_PASSO 4

#define COR_TEXTO_PREENCHIMENTO 16
#define COR_TEXTO_CONTORNO      2

#define TEXTO_DUPLICAR "DUPLICAR"
#define TEXTO_APAGAR "APAGAR"
#define COR_TEXTO_VERMELHO 244
#define COR_VERMELHO_RGB RGB15(27, 5, 5)
#define ICONE_Y_BASE 240
#define ICONE_LADO 13
#define ESPACO_ICONE 4

static const u8 ICONE_Y[13 * 13] = {
    0, 0, 0, 0, 240, 240, 240, 240, 242, 0, 0, 0, 0,
    0, 0, 242, 240, 240, 240, 240, 240, 240, 240, 242, 0, 0,
    0, 242, 240, 240, 240, 240, 240, 240, 240, 240, 240, 0, 0,
    0, 240, 240, 240, 243, 240, 240, 240, 243, 240, 240, 240, 0,
    240, 240, 240, 240, 241, 240, 240, 243, 241, 240, 240, 240, 242,
    240, 240, 240, 240, 240, 241, 240, 241, 240, 240, 240, 240, 242,
    240, 240, 240, 240, 240, 240, 241, 240, 240, 240, 240, 240, 242,
    240, 240, 240, 240, 240, 242, 241, 240, 240, 240, 240, 240, 242,
    240, 240, 240, 240, 240, 242, 241, 240, 240, 240, 240, 240, 242,
    0, 240, 240, 240, 240, 240, 243, 240, 240, 240, 240, 240, 0,
    0, 242, 240, 240, 240, 240, 240, 240, 240, 240, 240, 242, 0,
    0, 0, 242, 240, 240, 240, 240, 240, 240, 242, 242, 0, 0,
    0, 0, 0, 0, 242, 242, 242, 242, 242, 0, 0, 0, 0
};
static const u16 ICONE_Y_CORES[4] = { 8456, 32767, 0, 21140 };
static const u8 ICONE_X[13 * 13] = {
    0, 0, 0, 0, 0, 240, 240, 240, 242, 0, 0, 0, 0,
    0, 0, 0, 240, 240, 240, 240, 240, 240, 240, 242, 0, 0,
    0, 0, 240, 240, 240, 240, 240, 240, 240, 240, 240, 242, 0,
    0, 240, 240, 240, 240, 240, 240, 240, 240, 240, 240, 240, 0,
    0, 240, 240, 240, 240, 241, 240, 243, 241, 240, 240, 240, 242,
    242, 240, 240, 240, 240, 243, 241, 241, 240, 240, 240, 240, 240,
    240, 240, 240, 240, 240, 240, 241, 243, 240, 240, 240, 240, 240,
    242, 240, 240, 240, 240, 243, 241, 241, 240, 240, 240, 240, 242,
    0, 240, 240, 240, 240, 241, 240, 240, 241, 240, 240, 240, 242,
    0, 240, 240, 240, 240, 240, 240, 240, 243, 240, 240, 240, 0,
    0, 0, 240, 240, 240, 240, 240, 240, 240, 240, 240, 242, 0,
    0, 0, 0, 240, 240, 240, 240, 240, 240, 240, 242, 0, 0,
    0, 0, 0, 0, 242, 242, 242, 242, 242, 0, 0, 0, 0,
};

static u8 telaBaixoBuf[256 * 192];
static Modo modo = MODO_NORMAL;
static int rodape_dy = 0;
static int rodape_alvo = 0;
static int slot_selecionado = 0;
static MenuEstado estado_menu = MENU_ENTRA;
static Destino destino = DESTINO_TITULO;

typedef struct { int x, y; } Ponto;

static const Ponto SLOTS[3] = {
    { 44,  60 },
    { 128, 118 },
    { 212, 60 }
};

#define SLOT_RAIO 40
#define BOTAO_LADO 80
#define BOTAO_METADE 40
#define ANEL_RAIO 44

static inline void plotar(int x, int y, u8 indice)
{
    if ((unsigned)x < 256 && (unsigned)y < 192) {
        telaBaixoBuf[y * 256 + x] = indice;
    }
}

static inline u32 ler32(const u8 *p)
{
    return p[0] | (p[1] << 8) | (p[2] << 16) | ((u32)p[3] << 24);
}

static inline u16 ler16(const u8 *p)
{
    return p[0] | (p[1] << 8);
}

static const u8 *secao_nftr(const u8 *atual)
{
    u32 fim = ler32(FONTE + 8);
    const u8 *s = atual ? atual + ler32(atual + 4) : FONTE + 16;
    if ((u32)(s - FONTE) >= fim) return 0;
    return s;
}

static const u8 *achar_secao(const char *tag)
{
    for (const u8 *s = secao_nftr(0); s; s = secao_nftr(s)) {
        if (memcmp(s, tag, 4) == 0) return s;
    }
    return 0;
}

static int glifo_de(u16 c)
{
    for (const u8 *s = secao_nftr(0); s; s = secao_nftr(s)) {
        if (memcmp(s, "PAMC", 4) != 0) continue;
        const u8 *d = s + 8;
        u16 ini = ler16(d);
        u16 fim = ler16(d + 2);
        u32 tipo = ler32(d + 4);

        if (tipo == 2) {
            u16 n = ler16(d + 12);
            for (u16 k = 0; k < n; k++) {
                if (ler16(d + 14 + k * 4) == c) return ler16(d + 16 + k * 4);
            }
            continue;
        }
        if (c < ini || c > fim) continue;
        if (tipo == 0) return (int)(ler32(d + 12) + (c - ini));
        if (tipo == 1) {
            u16 g = ler16(d + 12 + (c - ini) * 2);
            if (g != 0xFFFF) return g;
        }
    }
    return -1;
}

static void medidas_glifo(int g, int *esquerda, int *avanco)
{
    const u8 *e = achar_secao("HDWC") + 16 + g * 3;
    *esquerda = (s8)e[0];
    *avanco   = (s8)e[2];
}

static int pixel_glifo(int g, int col, int lin)
{
    const u8 *pl = achar_secao("PLGC") + 8;
    int larg = pl[0];
    int tam  = ler16(pl + 2);
    const u8 *dados = pl + pl[4];
    int p = lin * larg + col;
    u8 byte = dados[g * tam + p / 4];
    return (byte >> (6 - 2 * (p % 4))) & 3;
}

static int largura_texto(const char *txt)
{
    int total = 0;
    for (; *txt; txt++) {
        int g = glifo_de((u8)*txt);
        if (g < 0) continue;
        int esquerda, avanco;
        medidas_glifo(g, &esquerda, &avanco);
        total += avanco;
    }
    return total;
}

static void desenhar_texto(int x, int y, const char *txt, u8 preenchimento, u8 contorno)
{
    const u8 CORES[4] = { 0, preenchimento, contorno, 0 };
    const u8 *pl = achar_secao("PLGC") + 8;
    int larg = pl[0];
    int alt  = pl[1];

    for (; *txt; txt++) {
        int g = glifo_de((u8)*txt);
        if (g < 0) continue;
        int esquerda, avanco;
        medidas_glifo(g, &esquerda, &avanco);
        for (int lin = 0; lin < alt; lin++) {
            for (int col = 0; col < larg; col++) {
                int v = pixel_glifo(g, col, lin);
                if (v) plotar(x + esquerda + col, y + lin, CORES[v]);
            }
        }
        x += avanco;
    }
}

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
    const u8 *src = (const u8 *)Button_templateBitmap;
    for (int y = 0; y < BOTAO_LADO; y++) {
        for (int x = 0; x < BOTAO_LADO; x++) {
            u8 indice = src[y * BOTAO_LADO + x];
            if (indice != 0) {
                plotar(cx - BOTAO_METADE + x, cy - BOTAO_METADE + y, indice);
            }
        }
    }
}

static void desenhar_icone(int x, int y, const u8 *icone)
{
    for (int lin = 0; lin < ICONE_LADO; lin++) {
        for (int col = 0; col < ICONE_LADO; col++) {
            u8 v = icone[lin * ICONE_LADO + col];
            if (v) plotar(x + col, y + lin, v);
        }
    }
}

static u16 cor_contorno_do_modo(void)
{
    if (modo == MODO_EXCLUIR) return COR_CONTORNO_EXCLUIR_RGB;
    if (modo == MODO_DUPLICAR) return COR_CONTORNO_DUPLICAR_RGB;
    return COR_CONTORNO_RGB;
}

static void aplicar_paleta_selecao(void)
{
    BG_PALETTE_SUB[COR_CONTORNO] = cor_contorno_do_modo();
    for (int k = 0; k < 4; k++) {
        BG_PALETTE_SUB[ICONE_Y_BASE + k] = ICONE_Y_CORES[k];
    }
    BG_PALETTE_SUB[COR_TEXTO_VERMELHO] = COR_VERMELHO_RGB;
}

static void aplicar_modo_selecao(void)
{
    const Imagem *topo = &SELECAO_TOPO;
    if (modo == MODO_EXCLUIR) topo = &SELECAO_EXCLUIR;
    if (modo == MODO_DUPLICAR) topo = &SELECAO_DUPLICAR;
    dmaCopy(topo->bitmap, bgGetGfxPtr(bgTopo), topo->bitmapLen);
    dmaCopy(topo->pal, BG_PALETTE, topo->palLen);
    aplicar_paleta_selecao();
}

static void montar_tela_selecao(int slot_atual)
{
    memcpy(telaBaixoBuf, SS_screenbgBBitmap, sizeof(telaBaixoBuf));

    for (int i = 0; i < 3; i++) {
        desenhar_botao(SLOTS[i].x, SLOTS[i].y);
        if (i == slot_atual) {
            desenhar_anel(SLOTS[i].x, SLOTS[i].y, ANEL_RAIO, COR_CONTORNO, 2);
        }
    }

    int altura = achar_secao("PLGC")[9];
    int y0 = 192 - altura - 8 + rodape_dy;

    int largura = largura_texto(TEXTO_DUPLICAR);
    int total = ICONE_LADO + ESPACO_ICONE + largura;
    int x0 = 256 - total - 8;
    desenhar_icone(x0, y0, ICONE_Y);
    desenhar_texto(x0 + ICONE_LADO + ESPACO_ICONE, y0, TEXTO_DUPLICAR, COR_TEXTO_PREENCHIMENTO, COR_TEXTO_CONTORNO);

    desenhar_icone(8, y0, ICONE_X);
    desenhar_texto(8 + ICONE_LADO + ESPACO_ICONE, y0, TEXTO_APAGAR, COR_TEXTO_VERMELHO, COR_TEXTO_CONTORNO);

    dmaCopy(telaBaixoBuf, bgGetGfxPtr(bgBaixo), sizeof(telaBaixoBuf));
}

static void animar_rodape(void)
{
    if (rodape_dy == rodape_alvo) return;
    int diff = rodape_alvo - rodape_dy;
    if (diff > RODAPE_PASSO) diff = RODAPE_PASSO;
    if (diff < -RODAPE_PASSO) diff = -RODAPE_PASSO;
    rodape_dy += diff;
    montar_tela_selecao(slot_selecionado);
}

static void menu_espera(u32 apertou)
{
    animar_rodape();

    bool pediu_x = (apertou & KEY_X) != 0;
    bool pediu_y = (apertou & KEY_Y) != 0;

    if (modo == MODO_NORMAL && (pediu_x || pediu_y) && !(pediu_x && pediu_y)) {
        modo = pediu_x ? MODO_EXCLUIR : MODO_DUPLICAR;
        rodape_alvo = RODAPE_DESCIDA;
        aplicar_modo_selecao();
        return;
    }

    if (apertou & KEY_B) {
        if (modo != MODO_NORMAL) {
            modo = MODO_NORMAL;
            rodape_alvo = 0;
            aplicar_modo_selecao();
        } else {
            destino = DESTINO_TITULO;
            estado_menu = MENU_SAI;
        }
        return;
    }

    if (apertou & KEY_A) {
        if (salvar_progresso(slot_selecionado) == 0) {
            salvar_definir_progresso(slot_selecionado, 1);
        }
        destino = DESTINO_SELECAO;
        estado_menu = MENU_SAI;
        return;
    }

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
}

void menu_iniciar(void)
{
    modo = MODO_NORMAL;
    rodape_dy = 0;
    rodape_alvo = 0;
    slot_selecionado = 0;
    estado_menu = MENU_ENTRA;

    dmaCopy(SELECAO_BAIXO.pal, BG_PALETTE_SUB, SELECAO_BAIXO.palLen);
    aplicar_modo_selecao();
    montar_tela_selecao(slot_selecionado);
}

MenuResultado menu_passo(u32 apertou)
{
    switch (estado_menu) {
    case MENU_ENTRA:
        if (fade_para(FADE_MAX)) {
            estado_menu = MENU_ESPERA;
        }
        break;

    case MENU_ESPERA:
        menu_espera(apertou);
        break;

    case MENU_SAI:
        if (fade_para(FADE_MIN)) {
            if (destino == DESTINO_TITULO) {
                return MENU_VOLTAR_TITULO;
            }
            estado_menu = MENU_ENTRA;
        }
        break;
    }
    return MENU_EM_ANDAMENTO;
}
