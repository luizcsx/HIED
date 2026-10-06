#pragma once

#include <nds.h>
#include <stdbool.h>
#include <string.h>

typedef struct {
    const void *bitmap;
    u32         bitmapLen;
    const void *pal;
    u32         palLen;
} Imagem;

#define IMAGEM(nome) { nome##Bitmap, nome##BitmapLen, nome##Pal, nome##PalLen }

#define FADE_MIN   (-16)
#define FADE_MAX   0
#define FADE_PASSO 2

extern int bgTopo;
extern int bgBaixo;
extern int nivel;

void iniciar_video(void);
void mostrar_telas(const Imagem *topo, const Imagem *baixo);
bool fade_para(int alvo);
