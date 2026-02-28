#include <GL/glew.h>
#include <GL/glut.h>
#include <cmath>
#include <cstdlib>
#include <cstdio> // Para o printf

#include "input/input.h"
#include "input/keystate.h"
#include "core/window.h"
#include "graphics/menu.h"
#include "core/game.h" 
#include "core/camera.h" // Necessário para saber a posição do jogador
#include "level/level.h" // Necessário para ler o mapa

// --- VARIÁVEIS EXTERNAS DO DEVOUR ---
extern int componentesCarregados;
extern int componentesQueimados;

void keyboard(unsigned char key, int, int)
{
    const GameState state = gameGetState();
    // ESC sai do jogo imediatamente em qualquer tela
    if (key == 27)
        std::exit(0);

    // --- MENU INICIAL ---
    if (state == GameState::MENU_INICIAL)
    {
        if (key == 13)
        {
            if (!menuMeltIsActive()) 
                menuMeltRequestStart();
        }
        return;
    }

    // --- GAME OVER ---
    if (state == GameState::GAME_OVER)
    {
        if (key == 13)
        { 
            gameReset();
            gameSetState(GameState::JOGANDO);
        }
        return;
    }

    // --- PAUSE ---
    if (state == GameState::PAUSADO)
    {
        if (key == 'p' || key == 'P')
        {
            gameSetState(GameState::JOGANDO);
        }
        return;
    }

    // --- JOGANDO ---
    if (state == GameState::JOGANDO)
    {
        if (key == 'p' || key == 'P')
        {
            gameSetState(GameState::PAUSADO);
            keyW = keyA = keyS = keyD = false;
            return;
        }

        // Controles de Jogo (WASD + F + E)
        switch (key)
        {
        case 'w': case 'W': keyW = true; break;
        case 's': case 'S': keyS = true; break;
        case 'a': case 'A': keyA = true; break;
        case 'd': case 'D': keyD = true; break;
            
        // --- LANTERNA ---
        case 'f':
        case 'F':
            flashlightOn = !flashlightOn; // Liga/Desliga a lanterna
            break;

        // --- QUEIMAR NO INCINERADOR (TECLA E) ---
        case 'e':
        case 'E':
            if (componentesCarregados > 0) {
                Level& lvl = gameLevel();
                float tile = lvl.metrics.tile;
                float offX = lvl.metrics.offsetX;
                float offZ = lvl.metrics.offsetZ;
                
                bool pertoDoIncinerador = false;

                // Varre os blocos ao redor do jogador para ver se tem um '9'
                int pX = (int)((camX - offX) / tile);
                int pZ = (int)((camZ - offZ) / tile);

                for (int bz = pZ - 1; bz <= pZ + 1; bz++) {
                    for (int bx = pX - 1; bx <= pX + 1; bx++) {
                        if (bz >= 0 && bz < lvl.map.getHeight() && bx >= 0 && bx < (int)lvl.map.data()[bz].size()) {
                            if (lvl.map.data()[bz][bx] == '9') {
                                pertoDoIncinerador = true;
                                break;
                            }
                        }
                    }
                }

                if (pertoDoIncinerador) {
                    componentesCarregados = 0;
                    componentesQueimados++;
                    printf("\n>>> SUCESSO! Peça destruida! Total: %d/10\n", componentesQueimados);
                } else {
                    printf("\n>>> ERRO: Voce nao esta no Incinerador (Bloco 9)!\n");
                }
            }
            break;
        }
    }
}

void keyboardUp(unsigned char key, int, int)
{
    switch (key)
    {
    case 'w': case 'W': keyW = false; break;
    case 's': case 'S': keyS = false; break;
    case 'a': case 'A': keyA = false; break;
    case 'd': case 'D': keyD = false; break;
    }

    if ((key == 13 || key == '\r') && (glutGetModifiers() & GLUT_ACTIVE_ALT))
    {
        altFullScreen();
    }
}

void mouseClick(int button, int state, int x, int y)
{
    // Removido o ataque! Em jogos de terror a gente só corre.
}