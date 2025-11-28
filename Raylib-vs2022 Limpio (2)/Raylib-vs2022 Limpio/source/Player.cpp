#include "Player.h"
#include "ResourceUtils.h"

Player::Player() {
    rect.width = kPlayerWidth;
    rect.height = kPlayerHeight;
    rect.x = kPlayerStartX;
    rect.y = (float)kScreenHeight - kPlayerStartYOffset - rect.height;
    velocity = { 0.0f, 0.0f };
    onGround = false;
    speed = kPlayerSpeed;
    jumpForce = kPlayerJumpForce;

    tex.id = 0;
    altTex.id = 0;
    altTexture = false;
    tint = WHITE;
}

float Player::prevBottom() const {
    return rect.y + rect.height;
}

void Player::loadTexture(const std::string& path) {
    loadTextureChecked(path, tex, "player");
}

void Player::loadAltTexture(const std::string& path) {
    loadTextureChecked(path, altTex, "player_alt");
}

void Player::unloadTextures() {
    if (tex.id != 0) ::UnloadTexture(tex);
    if (altTex.id != 0) ::UnloadTexture(altTex);
}

void Player::update(float dt, float gravity) {
    velocity.y += gravity * dt;
    rect.x += velocity.x * dt;
    rect.y += velocity.y * dt;
}

void Player::draw() const {
    Texture2D drawTex = tex;
    if (altTexture && altTex.id != 0) drawTex = altTex;

    if (drawTex.id != 0) {
        Rectangle src = { 0.0f, 0.0f, (float)drawTex.width, (float)drawTex.height };
        Rectangle dest = { rect.x, rect.y, rect.width, rect.height };
        Vector2 origin = { 0.0f, 0.0f };
        DrawTexturePro(drawTex, src, dest, origin, 0.0f, tint);
    }
    else {
        DrawRectangleRec(rect, tint);
        DrawRectangleLinesEx(rect, 1.0f, BLACK);
    }
}

void Player::toggleTint() {
    if (tint.r == 255 && tint.g == 255 && tint.b == 255) {
        tint = { 255, 50, 50, 255 };
    }
    else {
        tint = WHITE;
    }
}

void Player::toggleAltTexture() {
    altTexture = !altTexture;
}
