#ifndef PLATFORM_H
#define PLATFORM_H

#include "raylib.h"
#include <string>

class Platform {
public:
    Rectangle rect;
    Texture2D tex;

    Platform(float x = 0, float y = 0, float w = 0, float h = 0);

    void LoadTexture(const std::string& path);
    void UnloadTexture();

    void Draw() const;

    // Resolución de colisiones
    void ResolveHorizontalCollision(Rectangle& playerRect, float prevPlayerX) const;
    bool ResolveVerticalCollision(Rectangle& playerRect, Vector2& playerVel, float prevBottom, bool& outOnGround) const;
};

#endif // PLATFORM_H
