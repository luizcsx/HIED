#include "common.h"
#include "title.h"
#include "menuLdd.h"
#include "salvar.h"
#include "audio.h"

typedef enum {
    ESTADO_TITULO,
    ESTADO_MENU
} Estado;

int main(void)
{
    iniciar_video();
    salvar_iniciar();
    audio_iniciar();
    titulo_iniciar();

    Estado estado = ESTADO_TITULO;

    scanKeys();
    scanKeys();

    while (1) {
        swiWaitForVBlank();
        scanKeys();
        u32 apertou = keysDown();

        if (estado == ESTADO_TITULO) {
            if (titulo_passo(apertou) == TITULO_CONCLUIDO) {
                menu_iniciar();
                estado = ESTADO_MENU;
            }
        } else {
            if (menu_passo(apertou) == MENU_VOLTAR_TITULO) {
                titulo_iniciar();
                estado = ESTADO_TITULO;
            }
        }
    }

    return 0;
}
