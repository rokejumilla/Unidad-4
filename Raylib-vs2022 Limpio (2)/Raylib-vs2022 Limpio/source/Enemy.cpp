#include "Enemy.h"

// Nota: usamos ::GetRandomValue de raylib para aleatoriedad (no necesita <cstdlib> ni seed manual)
Enemy::Enemy() {
    rect = { 0, 0, 30.0f, 30.0f };
    speed = 60.0f; // base (puedes ajustar)
    dir = 1;
    leftBound = 0.0f;
    rightBound = 0.0f;
    tex.id = 0;
}

void Enemy::InitOnPlatform(const Platform& plat, float extraSpeed) {
    // bounds basados en la plataforma
    leftBound = plat.rect.x;
    rightBound = plat.rect.x + plat.rect.width - rect.width;

    // posición inicial aleatoria dentro de la plataforma (entre leftBound y rightBound)
    int minX = (int)leftBound;
    int maxX = (int)rightBound;
    if (maxX < minX) maxX = minX;
    int rndX = ::GetRandomValue(minX, maxX);
    rect.x = (float)rndX;

    // y encima de la plataforma
    rect.y = plat.rect.y - rect.height;

    // velocidad base + extra por plataforma + componente aleatorio
    // ajusta los números (base, rangoRand) a tu gusto
    float baseSpeed = 40.0f;                // velocidad base (más lenta)
    int   randOffset = ::GetRandomValue(-15, 30); // offset aleatorio en px/s (puede ser negativo)
    speed = baseSpeed + extraSpeed + (float)randOffset;
    if (speed < 20.0f) speed = 20.0f; // límite mínimo para que no se quede quieto

    // dirección inicial aleatoria
    dir = (::GetRandomValue(0, 1) == 0) ? -1 : 1;
}

void Enemy::LoadTexture(const std::string& path) {
    tex = ::LoadTexture(path.c_str());
}

void Enemy::UnloadTexture() {
    if (tex.id != 0) ::UnloadTexture(tex);
}

void Enemy::Update(float dt) {
    // movimiento simple ida/vuelta
    rect.x += speed * dir * dt;
    if (rect.x <= leftBound) {
        rect.x = leftBound;
        dir = 1;
    }
    else if (rect.x >= rightBound) {
        rect.x = rightBound;
        dir = -1;
    }
}

void Enemy::Draw() const {
    if (tex.id != 0) {
        Rectangle src;
        if (dir >= 0) src = { 0.0f, 0.0f, (float)tex.width, (float)tex.height };
        else src = { (float)tex.width, 0.0f, -(float)tex.width, (float)tex.height };

        Rectangle dest = { rect.x, rect.y, rect.width, rect.height };
        Vector2 origin = { 0.0f, 0.0f };
        DrawTexturePro(tex, src, dest, origin, 0.0f, WHITE);
    }
    else {
        DrawRectangleRec(rect, RED);
    }
}
