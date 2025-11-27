#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"
#include <string>

class Player {
public:
    Rectangle rect;
    Vector2 velocity;
    bool onGround;
    float speed;
    float jumpForce;

    // Texturas y apariencia
    Texture2D tex;
    Texture2D altTex;
    bool altTexture;   // si true usamos altTex en vez de tex (si altTex.id != 0)
    Color tint;        // tint aplicado al dibujar

    Player();

    float prevBottom() const;

    // carga/descarga texturas
    void LoadTexture(const std::string& path);
    void LoadAltTexture(const std::string& path);
    void UnloadTextures();

    // update / draw
    void Update(float dt, float gravity);
    void Draw() const;

    // utilidades
    void ToggleTint();        // alterna tint entre WHITE y rojo
    void ToggleAltTexture();  // alterna el flag altTexture
};

#endif // PLAYER_H
