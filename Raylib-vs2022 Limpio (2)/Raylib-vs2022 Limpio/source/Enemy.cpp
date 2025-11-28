#include "Enemy.h"
#include "ResourceUtils.h"

Enemy::Enemy() {
    rect = { 0, 0, kEnemyWidth, kEnemyHeight };
    speed = kEnemyBaseSpeed;
    dir = 1;
    leftBound = 0.0f;
    rightBound = 0.0f;
    tex.id = 0;
}

void Enemy::initOnPlatform(const Platform& plat, float extraSpeed) {
    leftBound = plat.rect.x;
    rightBound = plat.rect.x + plat.rect.width - rect.width;

    int minX = (int)leftBound;
    int maxX = (int)rightBound;
    if (maxX < minX) maxX = minX;
    int rndX = ::GetRandomValue(minX, maxX);
    rect.x = (float)rndX;

    rect.y = plat.rect.y - rect.height;

    int randOffset = ::GetRandomValue(kEnemyRandOffsetMin, kEnemyRandOffsetMax);
    speed = kEnemyBaseSpeed + extraSpeed + (float)randOffset;
    if (speed < kEnemyMinSpeed) speed = kEnemyMinSpeed;

    dir = (::GetRandomValue(0, 1) == 0) ? -1 : 1;
}

void Enemy::loadTexture(const std::string& path) {
    loadTextureChecked(path, tex, "enemy");
}

void Enemy::unloadTexture() {
    if (tex.id != 0) ::UnloadTexture(tex);
}

void Enemy::update(float dt) {
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

void Enemy::draw() const {
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
