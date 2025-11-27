#ifndef PLATFORM_H
#define PLATFORM_H

#include "raylib.h"
#include <string>
#include "Config.h"

class Platform {
public:
    Rectangle rect;
    Texture2D tex;

    Platform(float x = 0, float y = 0, float w = 0, float h = 0);

    void loadTexture(const std::string& path);
    void unloadTexture();

    void draw() const;

    void resolveHorizontalCollision(Rectangle& playerRect, float prevPlayerX) const;
    bool resolveVerticalCollision(Rectangle& playerRect, Vector2& playerVel, float prevBottom, bool& outOnGround) const;
};

#endif // PLATFORM_H
