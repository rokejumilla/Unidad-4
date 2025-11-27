#ifndef ENEMY_H
#define ENEMY_H

#include "raylib.h"
#include "Platform.h"
#include <string>
#include "Config.h"

class Enemy {
public:
    Rectangle rect;
    float speed;
    int dir; // -1 left, 1 right
    float leftBound;
    float rightBound;
    Texture2D tex;

    Enemy();

    void initOnPlatform(const Platform& plat, float extraSpeed = 0.0f);
    void loadTexture(const std::string& path);
    void unloadTexture();
    void update(float dt);
    void draw() const;
};

#endif // ENEMY_H
