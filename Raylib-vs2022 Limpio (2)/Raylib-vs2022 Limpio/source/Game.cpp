// Game.cpp
#include "Game.h"
#include "raylib.h"
#include <iostream>

// Rutas (ajusta si es necesario)
static const std::string DEFAULT_PLAYER_PATH = R"(C:\Users\santino\Downloads\Unidad-4\Raylib-vs2022 Limpio (2)\Raylib-vs2022 Limpio\player.png)";
static const std::string DEFAULT_GROUND_PATH = R"(C:\Users\santino\Downloads\Unidad-4\Raylib-vs2022 Limpio (2)\Raylib-vs2022 Limpio\mapa.png)";
static const std::string DEFAULT_ENEMY_PATH = R"(C:\Users\santino\Downloads\Unidad-4\Raylib-vs2022 Limpio (2)\Raylib-vs2022 Limpio\enemy.png)";
static const std::string DEFAULT_PLAYER_ALT = R"(C:\Users\santino\Downloads\Unidad-4\Raylib-vs2022 Limpio (2)\Raylib-vs2022 Limpio\player_alt.png)";
static const std::string DEFAULT_GOAL_PATH = R"(C:\Users\santino\Downloads\Unidad-4\Raylib-vs2022 Limpio (2)\Raylib-vs2022 Limpio\tesoro.png)";

Game::Game()
    : screenW(800), screenH(600), gravity(1000.0f),
    state(MENU), menuSelection(0), loseTimer(0.0f),
    elapsedTime(0.0f),
    playerPath(DEFAULT_PLAYER_PATH), groundPath(DEFAULT_GROUND_PATH),
    enemyPath(DEFAULT_ENEMY_PATH), playerAltPath(DEFAULT_PLAYER_ALT),
    goalPath(DEFAULT_GOAL_PATH)
{
    menuOptionBoxes[0] = { (float)(screenW / 2 - 80), (float)(screenH / 2 - 20), 160, 40 };
    menuOptionBoxes[1] = { (float)(screenW / 2 - 80), (float)(screenH / 2 + 30), 160, 40 };

    // plataformas
    platforms.emplace_back(0.0f, (float)(screenH - 60), 800.0f, 60.0f); // suelo
    platforms.emplace_back(100.0f, 460.0f, 110.0f, 15.0f);
    platforms.emplace_back(230.0f, 390.0f, 110.0f, 15.0f);
    platforms.emplace_back(360.0f, 320.0f, 110.0f, 15.0f);
    platforms.emplace_back(490.0f, 250.0f, 110.0f, 15.0f);
    platforms.emplace_back(620.0f, 180.0f, 150.0f, 25.0f);

    // objetivo (posición y tamaño)
    goal = { 690.0f, 98.0f, 80.0f, 80.0f };
    goalTex.id = 0;

    // crear enemigos para plataformas (excepto la 0), InitOnPlatform dará valores aleatorios
    for (size_t i = 1; i < platforms.size(); ++i) {
        Enemy e;
        e.rect.width = 30.0f;
        e.rect.height = 30.0f;
        e.InitOnPlatform(platforms[i], 5.0f * (float)i); // extra pequeño; InitOnPlatform añade randomness
        enemies.push_back(e);
    }

    // ajustar player Y inicial
    player.rect.y = (float)screenH - 81.0f - player.rect.height;
}

Game::~Game() {
    UnloadTexturesAndClose();
}

void Game::InitWindowAndTextures() {
    InitWindow(screenW, screenH, "Juego - Clases - Plataformas");
    SetTargetFPS(60);

    player.rect.y = (float)screenH - 81.0f - player.rect.height;

    // cargar texturas
    player.LoadTexture(playerPath);
    // si tienes una textura alternativa para el jugador, se cargará; si no, altTex.id será 0
    player.LoadAltTexture(playerAltPath);

    for (auto& p : platforms) p.LoadTexture(groundPath);
    for (auto& e : enemies) e.LoadTexture(enemyPath);

    // cargar textura del goal (tesoro)
    goalTex = ::LoadTexture(goalPath.c_str());
}

void Game::UnloadTexturesAndClose() {
    player.UnloadTextures();
    for (auto& p : platforms) p.UnloadTexture();
    for (auto& e : enemies) e.UnloadTexture();

    if (goalTex.id != 0) ::UnloadTexture(goalTex);
    // CloseWindow() se llama al final de Run()
}

void Game::Reset() {
    player.rect.x = 20.0f;
    player.rect.y = (float)screenH - 81.0f - player.rect.height;
    player.velocity = { 0,0 };
    player.onGround = false;
    player.tint = WHITE;
    player.altTexture = false;

    // Re-init enemigos (con nuevas propiedades aleatorias)
    enemies.clear();
    for (size_t i = 1; i < platforms.size(); ++i) {
        Enemy e;
        e.rect.width = 30.0f;
        e.rect.height = 30.0f;
        e.InitOnPlatform(platforms[i], 5.0f * (float)i);
        e.LoadTexture(enemyPath); // asegura que la textura se asigne incluso tras reset
        enemies.push_back(e);
    }

    loseTimer = 0.0f;
    elapsedTime = 0.0f;
}

void Game::Update(float dt) {
    if (state == PLAYING) {
        // INPUT HORIZONTAL
        float vx = 0;
        if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) vx += 1.0f;
        if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) vx -= 1.0f;
        player.velocity.x = vx * player.speed;

        // TECLA R: reiniciar nivel al instante
        if (IsKeyPressed(KEY_R)) {
            Reset();
            state = PLAYING;
            return;
        }

        // TECLA T: alternar tint / textura alternativa
        if (IsKeyPressed(KEY_T)) {
            player.ToggleTint();
            if (player.altTex.id != 0) player.ToggleAltTexture();
        }

        // SALTO
        if ((IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP)) && player.onGround) {
            player.velocity.y = player.jumpForce;
            player.onGround = false;
        }

        // aplicar gravedad
        player.velocity.y += gravity * dt;

        // --- colisiones con el patrón pedido ---
        float prevX = player.rect.x;
        float prevBottom = player.prevBottom();

        float nextX = player.rect.x + player.velocity.x * dt;
        float nextY = player.rect.y + player.velocity.y * dt;

        // mover horizontal y resolver colisión horizontal
        player.rect.x = nextX;
        for (auto& plat : platforms) {
            plat.ResolveHorizontalCollision(player.rect, prevX);
        }

        // mover vertical y resolver colisión vertical (one-way)
        player.rect.y = nextY;
        bool landed = false;
        if (player.velocity.y > 0.0f) {
            for (auto& plat : platforms) {
                bool hit = plat.ResolveVerticalCollision(player.rect, player.velocity, prevBottom, player.onGround);
                if (hit) { landed = true; break; }
            }
        }
        if (!landed) player.onGround = false;

        // límites pantalla
        if (player.rect.x < 0) player.rect.x = 0;
        if (player.rect.x + player.rect.width > screenW) player.rect.x = screenW - player.rect.width;
        if (player.rect.y + player.rect.height > screenH) {
            player.rect.y = screenH - player.rect.height;
            player.velocity.y = 0;
            player.onGround = true;
        }

        // actualizar enemigos (movimiento ida/vuelta)
        for (auto& e : enemies) e.Update(dt);

        // colisión jugador-enemigo -> LOST
        for (const auto& e : enemies) {
            if (CheckCollisionRecs(player.rect, e.rect)) {
                state = LOST;
                loseTimer = 0.0f;
                break;
            }
        }

        // colisión goal
        if (CheckCollisionRecs(player.rect, goal)) {
            state = WON;
        }

        if (IsKeyPressed(KEY_ESCAPE)) state = MENU;

        // actualizar cronómetro
        elapsedTime += dt;
    }
    else if (state == MENU) {
        // manejar input del menu
        if (IsKeyPressed(KEY_DOWN)) menuSelection = (menuSelection + 1) % 2;
        if (IsKeyPressed(KEY_UP)) menuSelection = (menuSelection + 2 - 1) % 2;
        Vector2 mousePoint = GetMousePosition();
        for (int i = 0; i < 2; ++i) if (CheckCollisionPointRec(mousePoint, menuOptionBoxes[i])) menuSelection = i;
    }
    else if (state == WON) {
        if (IsKeyPressed(KEY_R)) {
            Reset();
            state = PLAYING;
            return;
        }
        if (IsKeyPressed(KEY_ENTER) || IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            state = MENU;
        }
        if (IsKeyPressed(KEY_T)) {
            player.ToggleTint();
            if (player.altTex.id != 0) player.ToggleAltTexture();
        }
    }
    else if (state == LOST) {
        if (IsKeyPressed(KEY_R)) {
            Reset();
            state = PLAYING;
            return;
        }
        if (IsKeyPressed(KEY_ENTER) || IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            state = MENU;
        }
        if (IsKeyPressed(KEY_T)) {
            player.ToggleTint();
            if (player.altTex.id != 0) player.ToggleAltTexture();
        }
    }
}

void Game::Draw() const {
    BeginDrawing();
    ClearBackground(RAYWHITE);

    if (state == MENU) {
        DrawText("Mi Juego - Menu", 20, 20, 20, DARKGRAY);
        const std::vector<std::string> menuOptions = { "Entrar", "Salir" };
        for (int i = 0; i < 2; ++i) {
            Color boxColor = (menuSelection == i) ? LIGHTGRAY : GRAY;
            DrawRectangleRec(menuOptionBoxes[i], boxColor);
            DrawRectangleLinesEx(menuOptionBoxes[i], 2.0f, BLACK);
            int textW = MeasureText(menuOptions[i].c_str(), 20);
            DrawText(menuOptions[i].c_str(),
                (int)(menuOptionBoxes[i].x + menuOptionBoxes[i].width / 2 - textW / 2),
                (int)(menuOptionBoxes[i].y + 8),
                20, BLACK);
        }
        DrawText("Usa ARRIBA/ABAJO o el MOUSE. ENTER para confirmar.", 180, screenH - 40, 14, DARKGRAY);
        DrawText("R - Reiniciar nivel | T - Cambiar apariencia", 10, screenH - 20, 14, DARKGRAY);
    }
    else {
        ClearBackground(Color{ 82,170,168,255 });

        // cronómetro arriba al centro
        {
            int mins = (int)elapsedTime / 60;
            int secs = (int)elapsedTime % 60;
            int centis = (int)(elapsedTime * 100) % 100;
            const char* timeText = TextFormat("Tiempo: %02d:%02d.%02d", mins, secs, centis);
            int tw = MeasureText(timeText, 20);
            DrawText(timeText, screenW / 2 - tw / 2, 8, 20, BLACK);
        }

        // mostrar coordenadas del jugador (arriba izquierda)
        DrawText(TextFormat("X: %.1f  Y: %.1f", player.rect.x, player.rect.y), 10, 36, 18, BLACK);

        // dibujar plataformas
        for (const auto& p : platforms) p.Draw();

        // dibujar goal: si cargó la textura la dibujamos escalada; si no, fallback a rect blanco
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

        // dibujar enemigos
        for (const auto& e : enemies) e.Draw();

        // dibujar jugador
        player.Draw();

        DrawText("Izq/Dcha - Mover | Espacio - Saltar | ESC - Volver menu", 10, 60, 14, BLACK);
        DrawText("R - Reiniciar | T - Cambiar apariencia", 10, 76, 14, BLACK);

        if (state == WON) {
            DrawRectangle(120, 200, 560, 200, Fade(BLACK, 0.6f));
            DrawText("¡GANASTE!", screenW / 2 - MeasureText("¡GANASTE!", 40) / 2, 250, 40, RAYWHITE);

            // mostrar tiempo final
            int mins = (int)elapsedTime / 60;
            int secs = (int)elapsedTime % 60;
            int centis = (int)(elapsedTime * 100) % 100;
            const char* playedText = TextFormat("Tiempo: %02d:%02d.%02d", mins, secs, centis);
            int tw = MeasureText(playedText, 20);
            DrawText(playedText, screenW / 2 - tw / 2, 320, 20, RAYWHITE);

            DrawText("Presiona ENTER o clic para volver al menu. R para reiniciar.", screenW / 2 - MeasureText("Presiona ENTER o clic para volver al menu. R para reiniciar.", 20) / 2, 350, 20, RAYWHITE);
        }

        if (state == LOST) {
            DrawRectangle(0, 0, screenW, screenH, Fade(BLACK, 0.6f));
            DrawText("PERDISTE", screenW / 2 - MeasureText("PERDISTE", 60) / 2, screenH / 2 - 60, 60, RAYWHITE);

            // mostrar tiempo jugado
            int mins = (int)elapsedTime / 60;
            int secs = (int)elapsedTime % 60;
            int centis = (int)(elapsedTime * 100) % 100;
            const char* playedText = TextFormat("Tiempo jugado: %02d:%02d.%02d", mins, secs, centis);
            int tw = MeasureText(playedText, 20);
            DrawText(playedText, screenW / 2 - tw / 2, screenH / 2 + 10, 20, RAYWHITE);

            DrawText("Presiona ENTER o clic para volver al menu. R para reiniciar.", screenW / 2 - MeasureText("Presiona ENTER o clic para volver al menu. R para reiniciar.", 20) / 2, screenH / 2 + 50, 20, RAYWHITE);
        }
    }

    EndDrawing();
}

void Game::Run() {
    InitWindowAndTextures();

    Vector2 mousePoint;

    while (!WindowShouldClose() && state != EXITING) {
        float dt = GetFrameTime();

        // Manejo del menú (input básico)
        if (state == MENU) {
            // permitir navegar con teclas / ratón
            if (IsKeyPressed(KEY_DOWN)) menuSelection = (menuSelection + 1) % 2;
            if (IsKeyPressed(KEY_UP)) menuSelection = (menuSelection + 2 - 1) % 2;
            mousePoint = GetMousePosition();
            for (int i = 0; i < 2; ++i) if (CheckCollisionPointRec(mousePoint, menuOptionBoxes[i])) menuSelection = i;

            if (IsKeyPressed(KEY_ENTER) || IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                if (menuSelection == 0) {
                    Reset();
                    state = PLAYING;
                }
                else {
                    state = EXITING;
                }
            }
        }

        Update(dt);
        Draw();
    }

    UnloadTexturesAndClose();
    CloseWindow();
}
