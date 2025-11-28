#include "ResourceUtils.h"
#include "raylib.h"
#include <iostream>

bool loadTextureChecked(const std::string& path, Texture2D& outTex, const std::string& resourceName) {
    outTex = ::LoadTexture(path.c_str());

    if (outTex.id == 0) {
        // Log both to raylib (if available) and cerr
        TraceLog(LOG_WARNING, "[ResourceUtils] Failed to load texture '%s' from path: %s", resourceName.c_str(), path.c_str());
        std::cerr << "[WARN] ResourceUtils: Failed to load texture '" << resourceName
            << "' from path: " << path << std::endl;
        return false;
    }
    else {
        TraceLog(LOG_INFO, "[ResourceUtils] Loaded texture '%s' from %s (id=%i, %ix%i)",
            resourceName.c_str(), path.c_str(), outTex.id, outTex.width, outTex.height);
        std::cerr << "[INFO] ResourceUtils: Loaded texture '" << resourceName
            << "' from " << path << " (id=" << outTex.id << ", " << outTex.width << "x" << outTex.height << ")" << std::endl;
        return true;
    }
}
