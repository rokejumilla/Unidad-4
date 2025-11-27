#include "Player.h"

// Constructor
Player::Player() {
    rect.width = 30.0f;
    rect.height = 40.0f;
    rect.x = 20.0f;
    rect.y = 600 - 81.0f - rect.height; // valor por defecto; Game ajusta si hace falta
    velocity = { 0.0f, 0.0f };
    onGround = false;
    speed = 200.0f;
    jumpForce = -420.0f;

    tex.id = 0;
    altTex.id = 0;
    altTexture = false;
    tint = WHITE;
}

float Player::prevBottom() const {
    return rect.y + rect.height;
}

void Player::LoadTexture(const std::string& path) {
    tex = ::LoadTexture(path.c_str());
}

void Player::LoadAltTexture(const std::string& path) {
    altTex = ::LoadTexture(path.c_str());
}

void Player::UnloadTextures() {
    if (tex.id != 0) ::UnloadTexture(tex);
    if (altTex.id != 0) ::UnloadTexture(altTex);
}

void Player::Update(float dt, float gravity) {
    // movimiento horizontal ya debe venir puesto externamente en velocity.x
    velocity.y += gravity * dt;
    rect.x += velocity.x * dt;
    rect.y += velocity.y * dt;
}

void Player::Draw() const {
    Texture2D drawTex = tex;
    if (altTexture && altTex.id != 0) drawTex = altTex;

    if (drawTex.id != 0) {
        Rectangle src = { 0.0f, 0.0f, (float)drawTex.width, (float)drawTex.height };
        Rectangle dest = { rect.x, rect.y, rect.width, rect.height };
        Vector2 origin = { 0.0f, 0.0f };
        DrawTexturePro(drawTex, src, dest, origin, 0.0f, tint);
    }
    else {
        // si no hay textura, dibujar rectángulo con tint (fallback)
        DrawRectangleRec(rect, tint);
        DrawRectangleLinesEx(rect, 1.0f, BLACK);
    }
}

void Player::ToggleTint() {
    if (tint.r == 255 && tint.g == 255 && tint.b == 255) {
        tint = { 255, 50, 50, 255 }; // rojo
    }
    else {
        tint = WHITE;
    }
}

void Player::ToggleAltTexture() {
    altTexture = !altTexture;
}
