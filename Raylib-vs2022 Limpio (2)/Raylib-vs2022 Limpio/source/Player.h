#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"
#include <string>
#include "Config.h"

class Player {
public:
    Rectangle rect;
    Vector2 velocity;
    bool onGround;
    float speed;
    float jumpForce;

    Texture2D tex;
    Texture2D altTex;
    bool altTexture;
    Color tint;

    Player();

    float prevBottom() const;

    void loadTexture(const std::string& path);
    void loadAltTexture(const std::string& path);
    void unloadTextures();

    void update(float dt, float gravity);
    void draw() const;

    void toggleTint();
    void toggleAltTexture();
};

#endif // PLAYER_H
