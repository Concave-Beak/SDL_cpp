#include "./Engine.hh"

int main() {
    Engine* instance = Engine::GetEngineInstance();
    instance->Init();
    return instance->Run();
}
