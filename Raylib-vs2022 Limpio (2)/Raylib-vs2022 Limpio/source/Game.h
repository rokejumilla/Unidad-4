#ifndef GAME_H
#define GAME_H

#include "Player.h"
#include "Platform.h"
#include "Enemy.h"
#include <vector>
#include <string>
#include "Config.h"

class Game {
public:
    Game();
    ~Game();

    void run();

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

    Texture2D goalTex;

    float loseTimer;
    float elapsedTime;

    std::string playerPath;
    std::string groundPath;
    std::string enemyPath;
    std::string playerAltPath;
    std::string goalPath;

    void initWindowAndTextures();
    void unloadTexturesAndClose();
    void reset();
    void update(float dt);
    void draw() const;
};

#endif // GAME_H
