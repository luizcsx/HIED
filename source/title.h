#pragma once

#include "common.h"

typedef enum {
    TITULO_EM_ANDAMENTO,
    TITULO_CONCLUIDO
} TituloResultado;

void titulo_iniciar(void);
TituloResultado titulo_passo(u32 apertou);
