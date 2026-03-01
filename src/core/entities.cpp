#include "core/entities.h"
#include "core/game.h"
#include "core/camera.h"
#include "level/level.h"
#include <cmath>
#include <cstdlib> 
#include "audio/audio_system.h"

extern int componentesQueimados;
extern int componentesCarregados;

const float WANDER_SPEED_MULT = 0.4f; 
const float VISION_DISTANCE = 15.0f;  
const float VISION_ANGLE_COS = 0.5f;  

// ==========================================
// FUNÇÃO DE COLISÃO DO MONSTRO (Sincronizada com o Player)
// ==========================================
bool isWalkable(float nx, float nz)
{
    auto& lvl = gameLevel(); 
    if (lvl.map.getHeight() == 0) return true;

    // Puxa as métricas reais do mapa (Tamanho do Tile e os Offsets)
    const LevelMetrics &m = lvl.metrics;
    float tile = m.tile;

    // A MÁGICA ESTÁ AQUI: Subtraímos o offset igualzinho o Player faz!
    float localX = nx - m.offsetX;
    float localZ = nz - m.offsetZ;

    // Converte para a matriz com precisão absoluta
    int tx = (int)std::floor(localX / tile);
    int tz = (int)std::floor(localZ / tile);

    const auto& data = lvl.map.data();
    int mapH = (int)data.size();

    // Proteção contra falhas fora do mapa
    if (tz < 0 || tz >= mapH) return false;
    if (tx < 0 || tx >= (int)data[tz].size()) return false;

    char c = data[tz][tx];

    // Se for parede (1 ou 2), o bicho trava
    if (c == '1' || c == '2' || c == 'U' || c == 'I' || c == 'O' || c == 'Y') return false;

    return true;
}

// ==========================================
// MÁQUINA DE ESTADOS DA IA
// ==========================================
void updateEntities(float dt)
{
    auto& g = gameContext();
    auto& lvl = gameLevel();

    for (auto& en : lvl.enemies)
    {
        // Agora pulamos APENAS o que já está morto/coletado
        if (en.state == STATE_DEAD) continue; 

        // Calcula a distância do jogador até a entidade (seja monstro ou HD)
        float dx = camX - en.x;
        float dz = camZ - en.z;
        float dist = std::sqrt(dx * dx + dz * dz);

        // ==========================================
        // LÓGICA DE COLETA DOS HDs (TIPO 4)
        // ==========================================
        if (en.type == 4) 
        {
            // SÓ PODE PEGAR SE ESTIVER PERTO E DE MÃOS VAZIAS (componentesCarregados == 0)
            if (dist < 1.2f && componentesCarregados == 0) {
                en.state = STATE_DEAD;       // O HD some do chão
                componentesCarregados++;     // Vai pra sua mão

                audioPlayHDCollected(gameAudio());
            }
            // Como é um HD, ele não precisa patrulhar, então pulamos o resto do código
            continue; 
        }

        // ==========================================
        // MÁQUINA DE ESTADOS DA IA (Monstros e Bosses)
        // ==========================================
        float dirToPlayerX = 0.0f;
        float dirToPlayerZ = 0.0f;
        if (dist > 0.001f) {
            dirToPlayerX = dx / dist;
            dirToPlayerZ = dz / dist;
        }

        bool playerVisible = false;
        
        if (dist < VISION_DISTANCE) {
            float dotProduct = (en.dirX * dirToPlayerX) + (en.dirZ * dirToPlayerZ);
            // Ele sempre te vê se estiver perto, ou se estiver na frente dele
            if (dotProduct > VISION_ANGLE_COS || dist < 3.0f) {
                playerVisible = true;
            }
        }

        switch (en.state)
        {
            case STATE_IDLE:
            {
                float angle = (float)(rand() % 360) * 3.14159f / 180.0f;
                en.dirX = cosf(angle);
                en.dirZ = sinf(angle);
                en.animTimer = 3.0f + (float)(rand() % 300) / 100.0f;
                en.state = STATE_CHASE; 
                break;
            }

            case STATE_CHASE:
            {
                if (playerVisible) {
                    en.dirX = dirToPlayerX;
                    en.dirZ = dirToPlayerZ;

                    float speed = ENEMY_SPEED * (1.0f + (componentesQueimados * 0.20f));
                    float step = speed * dt;

                    if (isWalkable(en.x + en.dirX * step, en.z)) en.x += en.dirX * step;
                    if (isWalkable(en.x, en.z + en.dirZ * step)) en.z += en.dirZ * step;

                    if (dist < 1.2f) {
                        en.state = STATE_ATTACK;
                        en.attackCooldown = 0.8f; 
                    }
                } 
                else {
                    float step = ENEMY_SPEED * WANDER_SPEED_MULT * dt;
                    float lookAhead = 1.5f; 

                    if (isWalkable(en.x + (en.dirX * lookAhead), en.z)) {
                        en.x += en.dirX * step;
                    } else {
                        en.dirX *= -1.0f; 
                    }

                    if (isWalkable(en.x, en.z + (en.dirZ * lookAhead))) {
                        en.z += en.dirZ * step;
                    } else {
                        en.dirZ *= -1.0f; 
                    }

                    en.animTimer -= dt;

                    if (en.animTimer <= 0.0f) {
                        en.state = STATE_IDLE; 
                    }
                }
                break;
            }

            case STATE_ATTACK:
            {
                en.attackCooldown -= dt;
                
                if (dist > 1.5f) en.state = STATE_CHASE;
                
                if (en.attackCooldown <= 0.0f && dist <= 1.5f) {
                    g.player.health -= 35;
                    en.attackCooldown = 1.0f; 
                    g.player.damageAlpha = 1.0f; 
                }
                break;
            }
        }
    }
}