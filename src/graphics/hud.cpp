#include "graphics/hud.h"
#include "graphics/ui_text.h"
#include <GL/glew.h>
#include <GL/glut.h>
#include <string>
#include <cstdlib>

void drawDevourCounter(int w, int h, int queimados);

static void begin2D(int w, int h)
{
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, w, 0, h);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
}

static void end2D()
{
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    glMatrixMode(GL_MODELVIEW);
}

static void drawDamageOverlay(int w, int h, GLuint texDamage, float alpha)
{
    if (alpha <= 0.0f || texDamage == 0)
        return;

    begin2D(w, h);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindTexture(GL_TEXTURE_2D, texDamage);
    glColor4f(1, 1, 1, alpha);

    glBegin(GL_QUADS);
    glTexCoord2f(0, 1); glVertex2f(0, 0);
    glTexCoord2f(1, 1); glVertex2f((float)w, 0);
    glTexCoord2f(1, 0); glVertex2f((float)w, (float)h);
    glTexCoord2f(0, 0); glVertex2f(0, (float)h);
    glEnd();

    glDisable(GL_BLEND);

    end2D();
}

static void drawHealthOverlay(int w, int h, GLuint texHealth, float alpha)
{
    if (alpha <= 0.0f || texHealth == 0)
        return;

    begin2D(w, h);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindTexture(GL_TEXTURE_2D, texHealth);
    glColor4f(1, 1, 1, alpha);

    glBegin(GL_QUADS);
    glTexCoord2f(0, 1); glVertex2f(0, 0);
    glTexCoord2f(1, 1); glVertex2f((float)w, 0);
    glTexCoord2f(1, 0); glVertex2f((float)w, (float)h);
    glTexCoord2f(0, 0); glVertex2f(0, (float)h);
    glEnd();

    glDisable(GL_BLEND);

    end2D();
}

void hudRenderAll(
    int screenW,
    int screenH,
    const HudTextures& tex,
    const HudState& state,
    bool showCrosshair, // Mantido apenas por compatibilidade com as chamadas no game.cpp
    bool showWeapon,    // Mantido apenas por compatibilidade
    bool showDoomBar,   // Mantido apenas por compatibilidade
    int queimados)
{
    // Apenas desenha o contador de HDs e os efeitos visuais (dano/cura)
    drawDevourCounter(screenW, screenH, queimados);
    drawDamageOverlay(screenW, screenH, tex.texDamage, state.damageAlpha);
    drawHealthOverlay(screenW, screenH, tex.texHealthOverlay, state.healthAlpha);
}

void drawDevourCounter(int w, int h, int queimados)
{
    glPushAttrib(GL_ENABLE_BIT);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);

    begin2D(w, h);

    int restantes = 10 - queimados;
    std::string texto = "HDs CORROMPIDOS RESTANTES: " + std::to_string(restantes);

    // Escala do texto
    float scale = 0.00025f * h; 

    float x = (float)w - 650.0f; // Ajuste para caber o texto novo 
    float y = (float)h - 50.0f;

    // Fica vermelho quando só falta 1!
    if (restantes <= 1) glColor3f(1.0f, 0.0f, 0.0f);
    else glColor3f(1.0f, 1.0f, 1.0f);

    uiDrawStrokeText(x, y, (char*)texto.c_str(), scale);

    end2D();
    glPopAttrib();
}