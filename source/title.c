#include "title.h"
#include "HIED_screen.h"
#include "HIED_screenB.h"

static const Imagem TITULO_TOPO  = IMAGEM(HIED_screen);
static const Imagem TITULO_BAIXO = IMAGEM(HIED_screenB);

#define TECLAS_AVANCAR (KEY_A | KEY_START)

typedef enum {
    TIT_ENTRA,
    TIT_ESPERA,
    TIT_SAI
} TituloEstado;

static TituloEstado estado_titulo = TIT_ENTRA;

static bool tocou(void)
{
    touchPosition toque;
    touchRead(&toque);
    return (keysHeld() & KEY_TOUCH) && toque.px != 0 && toque.py != 0;
}

void titulo_iniciar(void)
{
    mostrar_telas(&TITULO_TOPO, &TITULO_BAIXO);
    estado_titulo = TIT_ENTRA;
}

TituloResultado titulo_passo(u32 apertou)
{
    switch (estado_titulo) {
    case TIT_ENTRA:
        if (fade_para(FADE_MAX)) {
            estado_titulo = TIT_ESPERA;
        }
        break;

    case TIT_ESPERA:
        if ((apertou & TECLAS_AVANCAR) || tocou()) {
            estado_titulo = TIT_SAI;
        }
        break;

    case TIT_SAI:
        if (fade_para(FADE_MIN)) {
            return TITULO_CONCLUIDO;
        }
        break;
    }
    return TITULO_EM_ANDAMENTO;
}
