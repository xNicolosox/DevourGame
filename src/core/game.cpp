#include <GL/glew.h> // GLEW SEMPRE PRIMEIRO
#include <GL/glut.h>
#include <cmath>
#include <cstdio>
#include <cstdlib>

#include "core/game_enums.h"
#include "core/game_state.h"
#include "core/game.h"
#include "level/level.h"
#include "core/camera.h"
#include "input/input.h"
#include "input/keystate.h"
#include "graphics/drawlevel.h"
#include "graphics/skybox.h"
#include "graphics/hud.h"
#include "graphics/menu.h"
#include "graphics/lighting.h"
#include "core/movement.h"
#include "core/player.h"
#include "core/entities.h"
#include "audio/audio_system.h"
#include "utils/assets.h"
#include "core/config.h"
#include "core/window.h"

// --- VARIÁVEIS DO DEVOUR ---
int componentesCarregados = 0;
int componentesQueimados = 0;

static HudTextures gHudTex;
static GameContext g;

constexpr int MAX_MAGAZINE = 12;

static GameAssets gAssets;
Level gLevel;
static AudioSystem gAudioSys;

GameContext &gameContext() { return g; }
AudioSystem &gameAudio() { return gAudioSys; }
Level &gameLevel() { return gLevel; }
GameState gameGetState() { return g.state; }
void gameSetState(GameState s) { g.state = s; }

void gameTogglePause()
{
    if (g.state == GameState::JOGANDO) g.state = GameState::PAUSADO;
    else if (g.state == GameState::PAUSADO) g.state = GameState::JOGANDO;
}

bool gameInit(const char *mapPath)
{
    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); 

    setupSunLightOnce();
    setupIndoorLightOnce();

    if (!loadAssets(gAssets)) return false;

    g.r.texChao = gAssets.texChao;
    g.r.texParede = gAssets.texParede;
    g.r.texSangue = gAssets.texSangue;
    g.r.texLava = gAssets.texLava;
    g.r.texChaoInterno = gAssets.texChaoInterno;
    g.r.texParedeInterna = gAssets.texParedeInterna;
    g.r.texTeto = gAssets.texTeto;
    g.r.texSkydome = gAssets.texSkydome;
    g.r.texMenuBG = gAssets.texMenuBG;
    g.r.texTelaWin = gAssets.texTelaWin;
    gHudTex.texHudFundo = gAssets.texHudFundo;
    gHudTex.texGunHUD = gAssets.texGunHUD;
    gHudTex.texGunDefault = gAssets.texGunDefault;
    gHudTex.texGunFire1 = gAssets.texGunFire1;
    gHudTex.texGunFire2 = gAssets.texGunFire2;
    gHudTex.texGunReload1 = gAssets.texGunReload1;
    gHudTex.texGunReload2 = gAssets.texGunReload2;
    gHudTex.texDamage = gAssets.texDamage;
    gHudTex.texHealthOverlay = gAssets.texHealthOverlay;

    for (int i = 0; i < 5; i++) {
        g.r.texEnemies[i] = gAssets.texEnemies[i];
        g.r.texEnemiesRage[i] = gAssets.texEnemiesRage[i];
        g.r.texEnemiesDamage[i] = gAssets.texEnemiesDamage[i];
    }

    g.r.texHealth = gAssets.texHealth;
    g.r.texAmmo = gAssets.texAmmo;
    g.r.progSangue = gAssets.progSangue;
    g.r.progLava = gAssets.progLava;

    if (!loadLevel(gLevel, mapPath, GameConfig::TILE_SIZE)) return false;

    applySpawn(gLevel, camX, camZ);
    camY = GameConfig::PLAYER_EYE_Y;

    glutKeyboardFunc(keyboard);
    glutKeyboardUpFunc(keyboardUp);
    glutPassiveMotionFunc(mouseMotion);
    glutSetCursor(GLUT_CURSOR_NONE);

    audioInit(gAudioSys, gLevel);

    g.state = GameState::MENU_INICIAL;
    g.time = 0.0f;
    g.player = PlayerState{};
    g.weapon = WeaponAnim{};
    
    componentesCarregados = 0;
    componentesQueimados = 0;

    return true;
}

void gameReset()
{
    g.player.health = 100;
    g.player.currentAmmo = 12;
    g.player.reserveAmmo = 25;
    g.player.damageAlpha = 0.0f;
    g.player.healthAlpha = 0.0f;
    g.weapon.state = WeaponState::W_IDLE;
    g.weapon.timer = 0.0f;

    componentesCarregados = 0;
    componentesQueimados = 0;

    applySpawn(gLevel, camX, camZ);
}

void gameUpdate(float dt)
{
    g.time += dt;

    // Se não estiver jogando, não processa física nem IA
    if (g.state != GameState::JOGANDO) return;

    atualizaMovimento();

    // Atualização de Áudio
    AudioListener L; L.pos = {camX, camY, camZ};
    float ry = yaw * 3.14159f / 180.0f; float rp = pitch * 3.14159f / 180.0f;
    L.forward = {cosf(rp) * sinf(ry), sinf(rp), -cosf(rp) * cosf(ry)};
    L.up = {0.0f, 1.0f, 0.0f}; L.vel = {0.0f, 0.0f, 0.0f};

    bool moving = (keyW || keyA || keyS || keyD);
    audioUpdate(gAudioSys, gLevel, L, dt, moving, g.player.health);

    // Efeitos de flash de dano/cura
    if (g.player.damageAlpha > 0.0f) {
        g.player.damageAlpha -= dt * 0.5f;
        if (g.player.damageAlpha < 0.0f) g.player.damageAlpha = 0.0f;
    }
    if (g.player.healthAlpha > 0.0f) {
        g.player.healthAlpha -= dt * 0.9f;
        if (g.player.healthAlpha < 0.0f) g.player.healthAlpha = 0.0f;
    }

    // --- ATUALIZA MONSTROS E ITENS (Apenas uma vez!) ---
    updateEntities(dt);

    // --- CHECAGEM DE VITÓRIA (Pegou os 10) ---
    if (componentesQueimados >= 10) {
        g.state = GameState::FASE_CONCLUIDA;
        glutSetCursor(GLUT_CURSOR_LEFT_ARROW); // Ou GLUT_CURSOR_INFO
    }

    // --- CHECAGEM DE DERROTA (Morreu) ---
    if (g.player.health <= 0) {
        g.state = GameState::GAME_OVER;
        g.player.damageAlpha = 1.0f;
    }
}

void drawWorld3D()
{
    glMatrixMode(GL_MODELVIEW); glLoadIdentity();
    glEnable(GL_TEXTURE_2D); glEnable(GL_LIGHTING); glEnable(GL_DEPTH_TEST);

    float radYaw = yaw * 3.14159265f / 180.0f;
    float radPitch = pitch * 3.14159265f / 180.0f;
    float dirX = cosf(radPitch) * sinf(radYaw);
    float dirY = sinf(radPitch);
    float dirZ = -cosf(radPitch) * cosf(radYaw);
    gluLookAt(camX, camY, camZ, camX + dirX, camY + dirY, camZ + dirZ, 0.0f, 1.0f, 0.0f);

    setSunDirectionEachFrame();
    drawLevel(gLevel.map, camX, camZ, dirX, dirZ, g.r, g.time);
    drawEntities(gLevel.enemies, gLevel.items, camX, camZ, dirX, dirZ, g.r);
}

void gameRender()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    HudState hs;
    hs.playerHealth = (int)g.player.health;
    hs.currentAmmo = g.player.currentAmmo;
    hs.reserveAmmo = g.player.reserveAmmo;
    hs.damageAlpha = g.player.damageAlpha;
    hs.healthAlpha = g.player.healthAlpha;
    hs.weaponState = g.weapon.state;

    if (g.state == GameState::MENU_INICIAL) {
        menuRender(janelaW, janelaH, g.time, "", "Pressione ENTER para Sobreviver", g.r);
    }
    else if (g.state == GameState::GAME_OVER) {
        drawWorld3D();
        menuRender(janelaW, janelaH, g.time, "SISTEMA CORROMPIDO", "Pressione ENTER para Reiniciar", g.r);
    }
    else if (g.state == GameState::PAUSADO) {
        drawWorld3D();
        hudRenderAll(janelaW, janelaH, gHudTex, hs, true, false, true, componentesQueimados);
        pauseMenuRender(janelaW, janelaH, g.time);
    }else if (g.state == GameState::FASE_CONCLUIDA) {
        drawWorld3D(); // Desenha o jogo parado ao fundo
        
        // --- O TRUQUE MÁGICO ---
        // Salva a imagem original do menu
        GLuint fundoOriginal = g.r.texMenuBG; 
        
        // Coloca a imagem de vitória no lugar
        g.r.texMenuBG = g.r.texTelaWin; 

        // Chama o menu (agora ele vai desenhar a telaWin no fundo!)
        menuRender(janelaW, janelaH, g.time, "", "Pressione ENTER para ir para a Fase 2", g.r);
        
        // Devolve a imagem original para não bugar o Menu Inicial depois
        g.r.texMenuBG = fundoOriginal; 
    }
    else { // JOGANDO
        drawWorld3D();
        
        // --- HUD LIGADO PARA MOSTRAR O CONTADOR ---
        // Parâmetros: largura, altura, texturas, estado, mira, arma, barra, queimados
        hudRenderAll(janelaW, janelaH, gHudTex, hs, true, false, true, componentesQueimados);
        
        menuMeltRenderOverlay(janelaW, janelaH, g.time);
    }
    glutSwapBuffers();
}