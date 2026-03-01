#include "utils/assets.h"
#include "graphics/texture.h"
#include "graphics/shader.h"
#include <cstdio>

bool loadAssets(GameAssets &a)
{
    // --- TELAS ---
    a.texMenuBG = carregaTextura("assets/menu_bg.png");
    a.texTelaWin = carregaTextura("assets/telaWin.png");
    a.texTelaFinal = carregaTextura("assets/gamezerado.png");

    // --- CENÁRIO E SHADERS ---
    a.texParede = carregaTextura("assets/091.png");
    a.texLava = carregaTextura("assets/179.png");
    a.texChaoInterno = carregaTextura("assets/100.png");
    a.texParedeInterna = carregaTextura("assets/060.png");
    a.texTeto = carregaTextura("assets/081.png");

    a.progSangue = criaShader("shaders/blood.vert", "shaders/blood.frag");
    a.progLava = criaShader("shaders/lava.vert", "shaders/lava.frag");

    // --- BOSS 1 ('J') - Júlio ---
    a.texEnemiesRage[0] = carregaTextura("assets/enemies/enemyRage.png");

    a.texEnemiesRage[1] = carregaTextura("assets/enemies/enemyRage2.png");

    // --- BOSS 3 ('M') - Marco Leal ---
    a.texEnemiesRage[2] = carregaTextura("assets/enemies/enemyRage3.png");

    // --- INIMIGO EXTRA ('K') ---
    
    // --- COLETÁVEL: HD ('H') ---
    a.texEnemies[4] = carregaTextura("assets/hd.png");

    // --- OVERLAYS DE TELA (Efeitos) ---
    a.texDamage = carregaTextura("assets/damage.png");

    // --- VERIFICAÇÃO DE ERROS ---
    // (Removidos os checks de armas e itens de chão que não usamos mais)
    if ( !a.texParede ||  !a.texLava || !a.texDamage || !a.texMenuBG  ||
        !a.texEnemiesRage[0] ||
        !a.texEnemiesRage[1] ||
        !a.texEnemiesRage[2])
    {
        std::printf("ERRO: falha ao carregar algum asset (textura/shader).\n");
        return false;
    }
    
    return true;
}