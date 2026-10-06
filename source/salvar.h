#pragma once

#include <nds.h>

#define SALVAR_SLOTS 3

void salvar_iniciar(void);
u8 salvar_progresso(int slot);
void salvar_definir_progresso(int slot, u8 valor);
