#ifndef RESOURCE_UTILS_H
#define RESOURCE_UTILS_H

#include "raylib.h"
#include <string>

// Carga una textura y hace logging claro si falla.
// Devuelve true si la carga tuvo éxito.
bool loadTextureChecked(const std::string& path, Texture2D& outTex, const std::string& resourceName);

#endif // RESOURCE_UTILS_H
