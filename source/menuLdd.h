#pragma once

#include "common.h"

typedef enum {
    MENU_EM_ANDAMENTO,
    MENU_VOLTAR_TITULO
} MenuResultado;

void menu_iniciar(void);
MenuResultado menu_passo(u32 apertou);
