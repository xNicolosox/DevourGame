#include "core/entities.h"
#include "core/game.h"
#include "core/camera.h"
#include "audio/audio_system.h"
#include <cmath>
#include <cstdlib> 
#include <cstdio>  

// --- VARIÁVEIS EXTERNAS DO DEVOUR ---
extern int componentesCarregados;
extern int componentesQueimados;

// MANTIDO: Função de colisão original do seu motor
bool isWalkable(float x, float z)
{
    auto& lvl = gameLevel();
    float tile = lvl.metrics.tile;
    float offX = lvl.metrics.offsetX;
    float offZ = lvl.metrics.offsetZ;

    int tx = (int)((x - offX) / tile);
    int tz = (int)((z - offZ) / tile);

    const auto& data = lvl.map.data();

    if (tz < 0 || tz >= (int)data.size()) return false;
    if (tx < 0 || tx >= (int)data[tz].size()) return false;

    char c = data[tz][tx];
    if (c == '1' || c == '2') return false; 

    return true;
}

void updateEntities(float dt)
{
    auto& g = gameContext();
    auto& lvl = gameLevel();
    auto& audio = gameAudio();

    for (auto& en : lvl.enemies)
    {
        if (en.state == STATE_DEAD) continue;

        if (en.hurtTimer > 0.0f) en.hurtTimer -= dt;

        float dx = camX - en.x;
        float dz = camZ - en.z;
        float dist = std::sqrt(dx * dx + dz * dz);

        // =============================================================
        // 1. O BOSS PERSEGUIDOR (Tipo 2 = Marco Leal / Letra 'M')
        // =============================================================
// =============================================================
        // 1. O BOSS PERSEGUIDOR (Tipo 2 = Marco Leal / Letra 'M')
        // =============================================================
        if (en.type == 2) 
        {
            // --- CÁLCULO DE DIFICULDADE ---
            // Velocidade base: ENEMY_SPEED * 0.7f
            // Aumento por peça: 20% de ganho por cada peça queimada
            float baseSpeed = ENEMY_SPEED * 0.7f;
            float speedBoost = 1.0f + (componentesQueimados * 0.20f); 
            float moveStep = baseSpeed * speedBoost * dt;

            // Se o Marco estiver muito rápido, vamos dar um limite (Cap) para não quebrar o jogo
            float maxSpeedLimit = ENEMY_SPEED * 3.0f;
            if (moveStep > maxSpeedLimit * dt) moveStep = maxSpeedLimit * dt;

            if (dist > 0.8f) 
            {
                en.state = STATE_CHASE;
                float dirX = dx / dist;
                float dirZ = dz / dist;

                float nextX = en.x + dirX * moveStep;
                if (isWalkable(nextX, en.z)) en.x = nextX;

                float nextZ = en.z + dirZ * moveStep;
                if (isWalkable(en.x, nextZ)) en.z = nextZ;
            }
            else 
            {
                // Ataque do Boss
                en.state = STATE_ATTACK;
                en.attackCooldown -= dt;
                if (en.attackCooldown <= 0.0f)
                {
                    g.player.health -= 35; 
                    en.attackCooldown = 0.8f; // Ataca um pouco mais rápido que os mobs comuns
                    g.player.damageAlpha = 1.0f; 
                    audioPlayHurt(audio);
                }
            }
        }
        // =============================================================
        // 2. COLETÁVEIS (Tipo 0 = Júlio, Tipo 1 = Peças / Letras 'J' e 'G')
        // =============================================================
        else
        {
            // Eles ficam zanzando aleatoriamente pelo mapa
            en.respawnTimer -= dt;
            if (en.respawnTimer <= 0.0f)
            {
                float angle = (rand() % 360) * 3.14159f / 180.0f;
                en.dirX = cos(angle);
                en.dirZ = sin(angle);
                en.respawnTimer = 1.5f + (rand() % 2); 
            }

            float moveStep = (ENEMY_SPEED * 0.45f) * dt;
            float nextX = en.x + en.dirX * moveStep;
            float nextZ = en.z + en.dirZ * moveStep;

            if (isWalkable(nextX, en.z)) en.x = nextX; else en.dirX *= -1.0f;
            if (isWalkable(en.x, nextZ)) en.z = nextZ; else en.dirZ *= -1.0f;

            // Se o jogador encostar neles, eles são "coletados"
            if (dist < 1.2f && componentesCarregados == 0)
            {
                en.state = STATE_DEAD; 
                componentesCarregados = 1;
                printf("\n>>> RECOLHIDO! Corra para o Altar (Bloco 9)!\n");
            }
        }
    }

    // --- ITENS DE CURA ---
    for (auto& item : lvl.items)
    {
        if (!item.active) {
            item.respawnTimer -= dt;
            if (item.respawnTimer <= 0.0f) item.active = true;
            continue;
        }
        float dx = camX - item.x;
        float dz = camZ - item.z;
        if (dx * dx + dz * dz < 1.0f) {
            item.active = false;
            if (item.type == ITEM_HEALTH) {
                g.player.health = std::min(100.0f, g.player.health + 50.0f);
                g.player.healthAlpha = 1.0f;
            }
        }
    }
}