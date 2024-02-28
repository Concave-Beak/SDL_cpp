#include "./Engine.hh"
int main() {
    Engine* instance = Engine::GetEngineInstance();
    instance->Run();
}
