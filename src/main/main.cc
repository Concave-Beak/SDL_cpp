#define STB_IMAGE_IMPLEMENTATION
#include "../../lib/stbi/stb_image.h"
#undef STB_IMAGE_IMPLEMENTATION
// Need this so it doesn't compile the same functions twice
// TODO: Solve this

#include "../../include/main/Engine.hh"

int main() {
    Engine* instance = Engine::GetEngineInstance();
    instance->Init();
    return instance->Run();
}
