#ifndef GAME_H
#define GAME_H

#include "Player.h"
#include "Platform.h"
#include "Enemy.h"
#include <vector>
#include <string>

class Game {
public:
    Game();
    ~Game();

    void Run();

private:
    const int screenW;
    const int screenH;
    float gravity;

    enum ScreenState { MENU, PLAYING, WON, LOST, EXITING };
    ScreenState state;

    int menuSelection;
    Rectangle menuOptionBoxes[2];

    Player player;
    std::vector<Platform> platforms;
    std::vector<Enemy> enemies;
    Rectangle goal;

    // Textura del goal
    Texture2D goalTex;

    float loseTimer;

    // cronómetro
    float elapsedTime;

    // rutas a texturas (raw string literal)
    std::string playerPath;
    std::string groundPath;
    std::string enemyPath;
    std::string playerAltPath;
    std::string goalPath; // <-- ruta para tesoro.png

    void InitWindowAndTextures();
    void UnloadTexturesAndClose();
    void Reset();
    void Update(float dt);
    void Draw() const;
};

#endif // GAME_H
