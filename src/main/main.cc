#include "../../include/main/Engine.hh"

int main(int argc, char** argv) {
    Engine* instance = Engine::GetEngineInstance();
    if (argc > 1) {
        if (strcmp(argv[1], "--debug") == 0 || strcmp(argv[1], "-d") == 0) {
            instance->debugMode = true;
        }
    }
    instance->Init();
    return instance->Run();
}
