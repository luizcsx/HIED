#include "salvar.h"
#include <string.h>

#define SRAM_BASE ((volatile u8 *)0x0A000000)
#define SAVE_TAM 9
#define SAVE_VERSAO 1
#define SAVE_SLOT_INICIO 5

static u8 cache[SAVE_TAM];

static u8 soma_checksum(void)
{
    u8 s = 0;
    for (int i = 4; i < 8; i++) s += cache[i];
    return s;
}

static bool valido(void)
{
    if (cache[0] != 'H' || cache[1] != 'I' || cache[2] != 'E' || cache[3] != 'D') return false;
    if (cache[4] != SAVE_VERSAO) return false;
    return cache[8] == soma_checksum();
}

static void gravar(void)
{
    cache[8] = soma_checksum();
    for (int i = 0; i < SAVE_TAM; i++) {
        SRAM_BASE[i] = cache[i];
    }
}

void salvar_iniciar(void)
{
    for (int i = 0; i < SAVE_TAM; i++) {
        cache[i] = SRAM_BASE[i];
    }
    if (!valido()) {
        memset(cache, 0, sizeof(cache));
        cache[0] = 'H';
        cache[1] = 'I';
        cache[2] = 'E';
        cache[3] = 'D';
        cache[4] = SAVE_VERSAO;
        gravar();
    }
}

u8 salvar_progresso(int slot)
{
    if (slot < 0 || slot >= SALVAR_SLOTS) return 0;
    return cache[SAVE_SLOT_INICIO + slot];
}

void salvar_definir_progresso(int slot, u8 valor)
{
    if (slot < 0 || slot >= SALVAR_SLOTS) return;
    cache[SAVE_SLOT_INICIO + slot] = valor;
    gravar();
}
