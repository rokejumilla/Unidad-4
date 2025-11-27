#ifndef ENEMY_H
#define ENEMY_H

#include "raylib.h"
#include "Platform.h"
#include <string>

class Enemy {
public:
    Rectangle rect;
    float speed;
    int dir; // -1 left, 1 right
    float leftBound;
    float rightBound;
    Texture2D tex;

    Enemy();

    void InitOnPlatform(const Platform& plat, float extraSpeed = 0.0f);
    void LoadTexture(const std::string& path);
    void UnloadTexture();
    void Update(float dt);
    void Draw() const;
};

#endif // ENEMY_H
