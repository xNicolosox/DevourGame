#pragma once

#include "core/game_enums.h"
#include <GL/glew.h>


struct HudTextures
{
 
    GLuint texDamage = 0;
    GLuint texHealthOverlay = 0;
    GLuint texHD = 0;
};

struct HudState
{
    int playerHealth = 100;

    float damageAlpha = 0.0f;
    float healthAlpha = 0.0f;
    int componentesCarregados = 0;

};

void hudRenderAll(
    int screenW,
    int screenH,
    const HudTextures& tex,
    const HudState& state,
    bool showCrosshair,
    bool showWeapon,
    bool showDoomBar,
    int queimados);
