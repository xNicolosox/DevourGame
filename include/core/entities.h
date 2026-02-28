#pragma once

bool isWalkable(float x, float z);
void updateEntities(float dt);

// Configurações da IA (Focado apenas na velocidade do Boss)
const float ENEMY_SPEED = 2.5f;

enum EnemyState
{
    STATE_IDLE,
    STATE_CHASE,
    STATE_ATTACK,
    STATE_DEAD // Usado quando o boss "morre" ou quando o HD é coletado
};

struct Enemy
{
    int type;         // 0=Júlio, 1=Thiago, 2=Marco Leal, 4=HD
    float x, z;       // Posição no mundo
    float hp;         // Vida (mantido caso queira usar futuramente)
    EnemyState state; // Estado atual (IA)
    
    float startX, startZ;
    float respawnTimer;
    
    // Animação (Para quando você colocar sprites andando)
    int animFrame;
    float animTimer;

    // Combate / Dano
    float attackCooldown; // Tempo entre um ataque e outro do Boss
    float hurtTimer;      // Tempo de piscar vermelho
};

