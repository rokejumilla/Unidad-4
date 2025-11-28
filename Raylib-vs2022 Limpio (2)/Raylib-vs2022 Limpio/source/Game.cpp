#include "Game.h"
#include "raylib.h"
#include "ResourceUtils.h"
#include <iostream>

// Rutas (ajusta si es necesario)
static const std::string kDefaultPlayerPath = R"(C:\Users\santino\Downloads\Unidad-4\Raylib-vs2022 Limpio (2)\Raylib-vs2022 Limpio\player.png)";
static const std::string kDefaultGroundPath = R"(C:\Users\santino\Downloads\Unidad-4\Raylib-vs2022 Limpio (2)\Raylib-vs2022 Limpio\mapa.png)";
static const std::string kDefaultEnemyPath = R"(C:\Users\santino\Downloads\Unidad-4\Raylib-vs2022 Limpio (2)\Raylib-vs2022 Limpio\enemy.png)";
static const std::string kDefaultPlayerAlt = R"(C:\Users\santino\Downloads\Unidad-4\Raylib-vs2022 Limpio (2)\Raylib-vs2022 Limpio\player_alt.png)";
static const std::string kDefaultGoalPath = R"(C:\Users\santino\Downloads\Unidad-4\Raylib-vs2022 Limpio (2)\Raylib-vs2022 Limpio\tesoro.png)";

Game::Game()
    : screenW(kScreenWidth), screenH(kScreenHeight), gravity(kGravity),
    state(MENU), menuSelection(0), loseTimer(0.0f),
    elapsedTime(0.0f),
    playerPath(kDefaultPlayerPath), groundPath(kDefaultGroundPath),
    enemyPath(kDefaultEnemyPath), playerAltPath(kDefaultPlayerAlt),
    goalPath(kDefaultGoalPath)
{
    menuOptionBoxes[0] = { (float)(screenW / 2 - kMenuBoxWidth / 2), (float)(screenH / 2 - kMenuBoxHeight / 2), (float)kMenuBoxWidth, (float)kMenuBoxHeight };
    menuOptionBoxes[1] = { (float)(screenW / 2 - kMenuBoxWidth / 2), (float)(screenH / 2 + kMenuBoxHeight), (float)kMenuBoxWidth, (float)kMenuBoxHeight };

    platforms.emplace_back(0.0f, (float)(screenH - kGroundHeight), (float)kScreenWidth, kGroundHeight);
    platforms.emplace_back(100.0f, 460.0f, 110.0f, 15.0f);
    platforms.emplace_back(230.0f, 390.0f, 110.0f, 15.0f);
    platforms.emplace_back(360.0f, 320.0f, 110.0f, 15.0f);
    platforms.emplace_back(490.0f, 250.0f, 110.0f, 15.0f);
    platforms.emplace_back(620.0f, 180.0f, 150.0f, 25.0f);

    goal = { kGoalX, kGoalY, kGoalWidth, kGoalHeight };
    goalTex.id = 0;

    for (size_t i = 1; i < platforms.size(); ++i) {
        Enemy e;
        e.rect.width = kEnemyWidth;
        e.rect.height = kEnemyHeight;
        e.initOnPlatform(platforms[i], 5.0f * (float)i);
        enemies.push_back(e);
    }

    player.rect.y = (float)screenH - kPlayerStartYOffset - player.rect.height;
}

Game::~Game() {
    unloadTexturesAndClose();
}

void Game::initWindowAndTextures() {
    InitWindow(screenW, screenH, "Juego - Configurable");
    SetTargetFPS(60);

    player.rect.y = (float)screenH - kPlayerStartYOffset - player.rect.height;

    // Cargamos texturas con verificación
    player.loadTexture(playerPath);
    player.loadAltTexture(playerAltPath);

    for (auto& p : platforms) p.loadTexture(groundPath);
    for (auto& e : enemies) e.loadTexture(enemyPath);

    // goal
    loadTextureChecked(goalPath, goalTex, "goal");
}

void Game::unloadTexturesAndClose() {
    player.unloadTextures();
    for (auto& p : platforms) p.unloadTexture();
    for (auto& e : enemies) e.unloadTexture();

    if (goalTex.id != 0) ::UnloadTexture(goalTex);
}

void Game::reset() {
    player.rect.x = kPlayerStartX;
    player.rect.y = (float)screenH - kPlayerStartYOffset - player.rect.height;
    player.velocity = { 0,0 };
    player.onGround = false;
    player.tint = WHITE;
    player.altTexture = false;

    // Re-init enemigos (con nuevas propiedades aleatorias)
    enemies.clear();
    for (size_t i = 1; i < platforms.size(); ++i) {
        Enemy e;
        e.rect.width = kEnemyWidth;
        e.rect.height = kEnemyHeight;
        e.initOnPlatform(platforms[i], 5.0f * (float)i);
        e.loadTexture(enemyPath);
        enemies.push_back(e);
    }

    loseTimer = 0.0f;
    elapsedTime = 0.0f;
}

void Game::update(float dt) {
    if (state == PLAYING) {
        float vx = 0;
        if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) vx += 1.0f;
        if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) vx -= 1.0f;
        player.velocity.x = vx * player.speed;

        if (IsKeyPressed(KEY_R)) {
            reset();
            state = PLAYING;
            return;
        }

        if (IsKeyPressed(KEY_T)) {
            player.toggleTint();
            if (player.altTex.id != 0) player.toggleAltTexture();
        }

        if ((IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP)) && player.onGround) {
            player.velocity.y = player.jumpForce;
            player.onGround = false;
        }

        player.velocity.y += gravity * dt;

        float prevX = player.rect.x;
        float prevBottom = player.prevBottom();

        float nextX = player.rect.x + player.velocity.x * dt;
        float nextY = player.rect.y + player.velocity.y * dt;

        player.rect.x = nextX;
        for (auto& plat : platforms) {
            plat.resolveHorizontalCollision(player.rect, prevX);
        }

        player.rect.y = nextY;
        bool landed = false;
        if (player.velocity.y > 0.0f) {
            for (auto& plat : platforms) {
                bool hit = plat.resolveVerticalCollision(player.rect, player.velocity, prevBottom, player.onGround);
                if (hit) { landed = true; break; }
            }
        }
        if (!landed) player.onGround = false;

        if (player.rect.x < 0) player.rect.x = 0;
        if (player.rect.x + player.rect.width > screenW) player.rect.x = screenW - player.rect.width;
        if (player.rect.y + player.rect.height > screenH) {
            player.rect.y = screenH - player.rect.height;
            player.velocity.y = 0;
            player.onGround = true;
        }

        for (auto& e : enemies) e.update(dt);

        for (const auto& e : enemies) {
            if (CheckCollisionRecs(player.rect, e.rect)) {
                state = LOST;
                loseTimer = 0.0f;
                break;
            }
        }

        if (CheckCollisionRecs(player.rect, goal)) {
            state = WON;
        }

        if (IsKeyPressed(KEY_ESCAPE)) state = MENU;

        elapsedTime += dt;
    }
    else if (state == MENU) {
        if (IsKeyPressed(KEY_DOWN)) menuSelection = (menuSelection + 1) % 2;
        if (IsKeyPressed(KEY_UP)) menuSelection = (menuSelection + 2 - 1) % 2;
        Vector2 mousePoint = GetMousePosition();
        for (int i = 0; i < 2; ++i) if (CheckCollisionPointRec(mousePoint, menuOptionBoxes[i])) menuSelection = i;
    }
    else if (state == WON) {
        if (IsKeyPressed(KEY_R)) {
            reset();
            state = PLAYING;
            return;
        }
        if (IsKeyPressed(KEY_ENTER) || IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            state = MENU;
        }
        if (IsKeyPressed(KEY_T)) {
            player.toggleTint();
            if (player.altTex.id != 0) player.toggleAltTexture();
        }
    }
    else if (state == LOST) {
        if (IsKeyPressed(KEY_R)) {
            reset();
            state = PLAYING;
            return;
        }
        if (IsKeyPressed(KEY_ENTER) || IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            state = MENU;
        }
        if (IsKeyPressed(KEY_T)) {
            player.toggleTint();
            if (player.altTex.id != 0) player.toggleAltTexture();
        }
    }
}

void Game::draw() const {
    BeginDrawing();
    ClearBackground(RAYWHITE);

    if (state == MENU) {
        DrawText("Mi Juego - Menu", 20, 20, kMenuTextSize, kMenuTextColor);
        const std::vector<std::string> menuOptions = { "Entrar", "Salir" };
        for (int i = 0; i < 2; ++i) {
            Color boxColor = (menuSelection == i) ? kMenuBoxSelected : kMenuBoxDefault;
            DrawRectangleRec(menuOptionBoxes[i], boxColor);
            DrawRectangleLinesEx(menuOptionBoxes[i], 2.0f, BLACK);
            int textW = MeasureText(menuOptions[i].c_str(), kMenuTextSize);
            DrawText(menuOptions[i].c_str(),
                (int)(menuOptionBoxes[i].x + menuOptionBoxes[i].width / 2 - textW / 2),
                (int)(menuOptionBoxes[i].y + 8),
                kMenuTextSize, BLACK);
        }
        DrawText("Usa ARRIBA/ABAJO o el MOUSE. ENTER para confirmar.", 180, screenH - 40, kInfoTextSize, kMenuTextColor);
        DrawText("R - Reiniciar nivel | T - Cambiar apariencia", 10, screenH - 20, kInfoTextSize, kMenuTextColor);
    }
    else {
        ClearBackground(kBackgroundColor);

        {
            int mins = (int)elapsedTime / 60;
            int secs = (int)elapsedTime % 60;
            int centis = (int)(elapsedTime * 100) % 100;
            const char* timeText = TextFormat("Tiempo: %02d:%02d.%02d", mins, secs, centis);
            int tw = MeasureText(timeText, kTimerTextSize);
            DrawText(timeText, screenW / 2 - tw / 2, 8, kTimerTextSize, BLACK);
        }

        DrawText(TextFormat("X: %.1f  Y: %.1f", player.rect.x, player.rect.y), 10, 36, kCoordsTextSize, BLACK);

        for (const auto& p : platforms) p.draw();

        if (goalTex.id != 0) {
            Rectangle src = { 0.0f, 0.0f, (float)goalTex.width, (float)goalTex.height };
            Rectangle dest = { goal.x, goal.y, goal.width, goal.height };
            Vector2 origin = { 0.0f, 0.0f };
            DrawTexturePro(goalTex, src, dest, origin, 0.0f, WHITE);
        }
        else {
            DrawRectangleRec(goal, WHITE);
            DrawRectangleLinesEx(goal, 2.0f, BLACK);
        }

        for (const auto& e : enemies) e.draw();

        player.draw();

        DrawText("Izq/Dcha - Mover | Espacio - Saltar | ESC - Volver menu", 10, 60, kHintTextSize, BLACK);
        DrawText("R - Reiniciar | T - Cambiar apariencia", 10, 76, kHintTextSize, BLACK);

        if (state == WON) {
            DrawRectangle(120, 200, 560, 200, Fade(BLACK, 0.6f));
            DrawText("¡GANASTE!", screenW / 2 - MeasureText("¡GANASTE!", 40) / 2, 250, 40, RAYWHITE);

            int mins = (int)elapsedTime / 60;
            int secs = (int)elapsedTime % 60;
            int centis = (int)(elapsedTime * 100) % 100;
            const char* playedText = TextFormat("Tiempo: %02d:%02d.%02d", mins, secs, centis);
            int tw = MeasureText(playedText, kInfoTextSize);
            DrawText(playedText, screenW / 2 - tw / 2, 320, kInfoTextSize, RAYWHITE);

            DrawText("Presiona ENTER o clic para volver al menu. R para reiniciar.", screenW / 2 - MeasureText("Presiona ENTER o clic para volver al menu. R para reiniciar.", kInfoTextSize) / 2, 350, kInfoTextSize, RAYWHITE);
        }

        if (state == LOST) {
            DrawRectangle(0, 0, screenW, screenH, Fade(BLACK, 0.6f));
            DrawText("PERDISTE", screenW / 2 - MeasureText("PERDISTE", 60) / 2, screenH / 2 - 60, 60, RAYWHITE);

            int mins = (int)elapsedTime / 60;
            int secs = (int)elapsedTime % 60;
            int centis = (int)(elapsedTime * 100) % 100;
            const char* playedText = TextFormat("Tiempo jugado: %02d:%02d.%02d", mins, secs, centis);
            int tw = MeasureText(playedText, kInfoTextSize);
            DrawText(playedText, screenW / 2 - tw / 2, screenH / 2 + 10, kInfoTextSize, RAYWHITE);

            DrawText("Presiona ENTER o clic para volver al menu. R para reiniciar.", screenW / 2 - MeasureText("Presiona ENTER o clic para volver al menu. R para reiniciar.", kInfoTextSize) / 2, screenH / 2 + 50, kInfoTextSize, RAYWHITE);
        }
    }

    EndDrawing();
}

void Game::run() {
    initWindowAndTextures();

    Vector2 mousePoint;

    while (!WindowShouldClose() && state != EXITING) {
        float dt = GetFrameTime();

        if (state == MENU) {
            if (IsKeyPressed(KEY_DOWN)) menuSelection = (menuSelection + 1) % 2;
            if (IsKeyPressed(KEY_UP)) menuSelection = (menuSelection + 2 - 1) % 2;
            mousePoint = GetMousePosition();
            for (int i = 0; i < 2; ++i) if (CheckCollisionPointRec(mousePoint, menuOptionBoxes[i])) menuSelection = i;

            if (IsKeyPressed(KEY_ENTER) || IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                if (menuSelection == 0) {
                    reset();
                    state = PLAYING;
                }
                else {
                    state = EXITING;
                }
            }
        }

        update(dt);
        draw();
    }

    unloadTexturesAndClose();
    CloseWindow();
}
