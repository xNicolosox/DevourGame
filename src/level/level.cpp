#include "level/level.h"
#include "core/config.h" 
#include <cstdio>

// Configurações básicas para spawn
static const float ENEMY_START_HP = 100.0f;

bool loadLevel(Level &lvl, const char *mapPath, float tileSize)
{
    // 1. Carrega o mapa de texto (paredes, chão)
    if (!lvl.map.load(mapPath))
    {
        std::printf("ERRO: nao foi possivel carregar o mapa: %s\n", mapPath);
        return false;
    }

    lvl.metrics = LevelMetrics::fromMap(lvl.map, tileSize);
    
    // Limpa entidades antigas se houver
    lvl.enemies.clear();

    // 2. Escaneia o mapa procurando Bosses e o HD
    const auto& data = lvl.map.data();
    int H = lvl.map.getHeight();

    for(int z = 0; z < H; z++)
    {
        const std::string& row = data[z];
        for(int x = 0; x < (int)row.size(); x++)
        {
            char c = row[x];
            float wx, wz;
            lvl.metrics.tileCenter(x, z, wx, wz);

            // --- LÓGICA DE SPAWN (Apenas Bosses e HDs) ---
            int enemyType = -1; // -1 significa "não é entidade"

            if (c == 'J') enemyType = 0;      // Boss 1: Júlio
            else if (c == 'T') enemyType = 1; // Boss 2: Thiago 
            else if (c == 'M') enemyType = 2; // Boss 3: Marco Leal 
            else if (c == 'G') enemyType = 3; // (Espaço para monstro extra)
            else if (c == 'H') enemyType = 4; // Item Coletável: HD

            if (enemyType != -1) 
            {
                Enemy e;
                e.type = enemyType; 

                e.x = wx;
                e.z = wz;

                // Salva a posição inicial e zera timers
                e.startX = wx; 
                e.startZ = wz;
                e.respawnTimer = 0.0f;

                e.hp = ENEMY_START_HP; // Bosses podem ter HP, mesmo que a gente não use
                e.state = STATE_IDLE;
                e.animFrame = 0;
                e.animTimer = 0;
                e.hurtTimer = 0.0f;
                e.attackCooldown = 0.0f; 

                lvl.enemies.push_back(e);
            }
   
        }
    }

    return true;
}

void applySpawn(const Level &lvl, float &camX, float &camZ)
{
    lvl.metrics.spawnPos(lvl.map, camX, camZ);
}