// mavis_input_msvc_fixed.cpp
#include "raylib.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

struct SpriteState {
    Vector2 pos;
    float scale;
    float rotation;
    Color tint;
    Vector2 velocity;
};

static float Clampf(float v, float a, float b) { if (v < a) return a; if (v > b) return b; return v; }

int main(void)
{
    const int screenW = 800;
    const int screenH = 600;
    InitWindow(screenW, screenH, "¡Despierta, Mavis! - MSVC friendly");
    SetTargetFPS(60);

    srand((unsigned)time(NULL));

    // Intento de cargar textura (si falla, se dibuja rectángulo)
    const char* spritePath = "mavis.png";
    Texture2D tex = { 0 };
    if (FileExists(spritePath)) {
        tex = LoadTexture(spritePath);
    }

    // Audio
    InitAudioDevice();
    const char* soundPath = "beep.wav";
    Sound beep = { 0 };
    bool soundLoaded = false;
    if (FileExists(soundPath)) {
        beep = LoadSound(soundPath);
        soundLoaded = true;
    }

    // Player (Mavis)
    SpriteState player;
    player.pos.x = 150.0f; player.pos.y = 450.0f;
    player.scale = 1.0f;
    player.rotation = 0.0f;
    player.tint = WHITE;
    player.velocity.x = 0.0f; player.velocity.y = 0.0f;

    const float moveSpeed = 260.0f;

    // Salto simple (sin gravedad)
    bool jumping = false;
    float jumpTimer = 0.0f;
    const float jumpDuration = 0.6f;
    const float jumpHeight = 120.0f;
    const float groundY = 450.0f;

    bool showSystemMessage = true;
    const char* systemMessage = "SISTEMA: Mavis ha despertado... Presiona M para ocultar este mensaje.";

    // Botón para cambiar fondo (inicializar explicitamente)
    Rectangle btnRect;
    btnRect.x = (float)screenW - 180.0f; btnRect.y = 20.0f; btnRect.width = 160.0f; btnRect.height = 40.0f;
    Color btnColor;
    btnColor.r = 80; btnColor.g = 160; btnColor.b = 200; btnColor.a = 255;

    Color bg;
    bg.r = 110; bg.g = 100; bg.b = 215; bg.a = 255;

    char info[256];

    const int SOUND_KEY = KEY_S; // tecla para sonido

    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();

        // Movimiento continuo con flechas
        if (IsKeyDown(KEY_RIGHT)) player.pos.x += moveSpeed * dt;
        if (IsKeyDown(KEY_LEFT))  player.pos.x -= moveSpeed * dt;

        // Clamp en X
        player.pos.x = Clampf(player.pos.x, 16.0f, (float)screenW - 16.0f);

        // Salto con SPACE (IsKeyPressed)
        if (IsKeyPressed(KEY_SPACE) && !jumping) {
            jumping = true;
            jumpTimer = 0.0f;
        }

        if (jumping) {
            jumpTimer += dt;
            float t = jumpTimer / jumpDuration;
            if (t >= 1.0f) {
                jumping = false;
                jumpTimer = 0.0f;
                player.pos.y = groundY;
            }
            else {
                float offset = sinf(3.14159265f * t) * jumpHeight;
                player.pos.y = groundY - offset;
            }
        }
        else {
            player.pos.y = groundY;
        }

        // Reset con R
        if (IsKeyPressed(KEY_R)) {
            player.pos.x = 150.0f; player.pos.y = groundY;
            player.scale = 1.0f; player.rotation = 0.0f;
            jumping = false; jumpTimer = 0.0f;
            bg.r = 110; bg.g = 100; bg.b = 215; bg.a = 255;
        }

        // Toggle mensaje con M
        if (IsKeyPressed(KEY_M)) showSystemMessage = !showSystemMessage;

        // Sonido con S
        if (IsKeyPressed(SOUND_KEY) && soundLoaded) {
            PlaySound(beep);
        }

        // Mouse: clic en botón cambia color de fondo
        Vector2 mousePos = GetMousePosition();
        bool hoverBtn = CheckCollisionPointRec(mousePos, btnRect);
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if (hoverBtn) {
                bg.r = (unsigned char)GetRandomValue(30, 240);
                bg.g = (unsigned char)GetRandomValue(30, 240);
                bg.b = (unsigned char)GetRandomValue(30, 240);
            }
        }

        // Dibujo
        BeginDrawing();
        ClearBackground(bg);

        // Dibujo del "suelo"
        int groundLineY = (int)(groundY + 36); // aproximación al medio del sprite
        DrawLine(0, groundLineY + 2, screenW, groundLineY + 2, DARKGRAY);

        // Dibujar player: con textura o rect
        if (tex.id != 0) {
            Rectangle src = { 0.0f, 0.0f, (float)tex.width, (float)tex.height };
            float drawW = tex.width * player.scale;
            float drawH = tex.height * player.scale;
            Rectangle dest;
            dest.width = drawW; dest.height = drawH;
            dest.x = player.pos.x - drawW * 0.5f;
            dest.y = player.pos.y - drawH * 0.5f;
            Vector2 origin;
            origin.x = drawW * 0.5f; origin.y = drawH * 0.5f;
            DrawTexturePro(tex, src, dest, origin, player.rotation, player.tint);
        }
        else {
            float w = 48 * player.scale;
            float h = 72 * player.scale;
            Rectangle r;
            r.x = player.pos.x - w * 0.5f; r.y = player.pos.y - h * 0.5f; r.width = w; r.height = h;
            Vector2 origin; origin.x = w * 0.5f; origin.y = h * 0.5f;
            DrawRectanglePro(r, origin, player.rotation, player.tint);
            DrawText("MAVIS", (int)(player.pos.x - 18), (int)(player.pos.y - 6), 10, BLACK);
        }

        // Botón (resaltado si hover)
        Color actualBtn = btnColor;
        int tempR = (int)actualBtn.r + (hoverBtn ? 40 : 0);
        int tempG = (int)actualBtn.g + (hoverBtn ? 40 : 0);
        int tempB = (int)actualBtn.b + (hoverBtn ? 40 : 0);
        actualBtn.r = (unsigned char)Clampf((float)tempR, 0.0f, 255.0f);
        actualBtn.g = (unsigned char)Clampf((float)tempG, 0.0f, 255.0f);
        actualBtn.b = (unsigned char)Clampf((float)tempB, 0.0f, 255.0f);

        DrawRectangleRec(btnRect, actualBtn);
        DrawRectangleLinesEx(btnRect, 2, BLACK);
        DrawText("Cambiar BG (clic)", (int)(btnRect.x + 10), (int)(btnRect.y + 10), 14, BLACK);

        // Debug: coordenadas y estado
        sprintf_s(info, sizeof(info), "Pos: (%.0f, %.0f)  Jumping: %s  Speed: %.0f", player.pos.x, player.pos.y, jumping ? "SI" : "NO", moveSpeed);
        DrawText(info, 10, 10, 18, BLACK);

        DrawText("Flechas Izq/Dcha: mover   SPACE: saltar   R: reset   M: show/hide mensaje   S: sonido", 10, screenH - 26, 16, DARKGRAY);

        if (showSystemMessage) {
            Rectangle msgRect;
            msgRect.x = 20.0f; msgRect.y = 40.0f; msgRect.width = 560.0f; msgRect.height = 48.0f;
            DrawRectangleRec(msgRect, Fade(SKYBLUE, 0.25f));
            DrawText(systemMessage, 28, 52, 16, BLACK);
        }

        EndDrawing();
    }

    if (tex.id != 0) UnloadTexture(tex);
    if (soundLoaded) UnloadSound(beep);
    CloseAudioDevice();
    CloseWindow();

    return 0;
}
