#pragma once

#include "common.h"

typedef enum {
    MENU_EM_ANDAMENTO,
    MENU_VOLTAR_TITULO,
    MENU_ABRIR_CENA
} MenuResultado;

void menu_iniciar(void);
MenuResultado menu_passo(u32 apertou);
int menu_slot_escolhido(void);
