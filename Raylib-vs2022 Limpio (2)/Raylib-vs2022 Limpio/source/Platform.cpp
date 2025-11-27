#include "Platform.h"

Platform::Platform(float x, float y, float w, float h) {
    rect = { x, y, w, h };
    tex.id = 0;
}

void Platform::LoadTexture(const std::string& path) {
    tex = ::LoadTexture(path.c_str());
}

void Platform::UnloadTexture() {
    if (tex.id != 0) ::UnloadTexture(tex);
}

void Platform::Draw() const {
    if (tex.id != 0) {
        Rectangle src = { 0.0f, 0.0f, (float)tex.width, (float)tex.height };
        Rectangle dest = { rect.x, rect.y, rect.width, rect.height };
        Vector2 origin = { 0.0f, 0.0f };
        DrawTexturePro(tex, src, dest, origin, 0.0f, WHITE);
    }
    else {
        DrawRectangleRec(rect, BLACK);
    }
}

void Platform::ResolveHorizontalCollision(Rectangle& playerRect, float prevPlayerX) const {
    // comprobación de solapamiento vertical
    bool overlapY = !(playerRect.y + playerRect.height <= rect.y || playerRect.y >= rect.y + rect.height);
    if (!overlapY) return;

    float playerW = playerRect.width;

    // golpe desde la izquierda
    if (prevPlayerX + playerW <= rect.x && playerRect.x + playerW > rect.x) {
        playerRect.x = rect.x - playerW;
    }
    // golpe desde la derecha
    else if (prevPlayerX >= rect.x + rect.width && playerRect.x < rect.x + rect.width) {
        playerRect.x = rect.x + rect.width;
    }
}

bool Platform::ResolveVerticalCollision(Rectangle& playerRect, Vector2& playerVel, float prevBottom, bool& outOnGround) const {
    if (playerVel.y <= 0.0f) return false;

    bool overlapX = (playerRect.x + playerRect.width > rect.x) && (playerRect.x < rect.x + rect.width);
    float playerBottom = playerRect.y + playerRect.height;

    if (overlapX && prevBottom <= rect.y && playerBottom >= rect.y && playerRect.y < rect.y + rect.height) {
        playerRect.y = rect.y - playerRect.height;
        playerVel.y = 0.0f;
        outOnGround = true;
        return true;
    }
    return false;
}
