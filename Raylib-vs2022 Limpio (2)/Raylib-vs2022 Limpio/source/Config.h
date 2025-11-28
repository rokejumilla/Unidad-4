#ifndef CONFIG_H
#define CONFIG_H

#include "raylib.h"

// Pantalla
constexpr int kScreenWidth = 800;
constexpr int kScreenHeight = 600;

// Suelo / plataformas
constexpr float kGroundHeight = 60.0f;

// Jugador
constexpr float kPlayerWidth = 30.0f;
constexpr float kPlayerHeight = 40.0f;
constexpr float kPlayerStartX = 20.0f;
constexpr float kPlayerStartYOffset = kGroundHeight + 21.0f;
constexpr float kPlayerSpeed = 200.0f;
constexpr float kPlayerJumpForce = -420.0f;

// Física
constexpr float kGravity = 1000.0f;

// Enemigos
constexpr float kEnemyWidth = 30.0f;
constexpr float kEnemyHeight = 30.0f;
constexpr float kEnemyBaseSpeed = 40.0f;
constexpr float kEnemyMinSpeed = 20.0f;
constexpr int   kEnemyRandOffsetMin = -15;
constexpr int   kEnemyRandOffsetMax = 30;

// Goal (objetivo)
constexpr float kGoalWidth = 80.0f;
constexpr float kGoalHeight = 80.0f;
constexpr float kGoalX = 690.0f;
constexpr float kGoalY = 98.0f;

// Menú
constexpr int kMenuBoxWidth = 160;
constexpr int kMenuBoxHeight = 40;
constexpr int kMenuTextSize = 20;
constexpr int kInfoTextSize = 14;

// HUD / textos
constexpr int kTimerTextSize = 20;
constexpr int kCoordsTextSize = 18;
constexpr int kHintTextSize = 14;

// Colores
inline const Color kBackgroundColor = Color{ 82,170,168,255 };
inline const Color kMenuTextColor = DARKGRAY;
inline const Color kMenuBoxSelected = LIGHTGRAY;
inline const Color kMenuBoxDefault = GRAY;

#endif // CONFIG_H
