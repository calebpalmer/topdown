#ifndef TOPDOWNGAME_H
#define TOPDOWNGAME_H

#include <memory>

#include "CapEngine.h"
#include "hero.h"

namespace TopDown{

  class TopDownGame : public IEventSubscriber {
  public:
    struct CapEngine::Screen_t screenConfig;
    std::unique_ptr<CapEngine::VideoManager> vManager;
    std::unique_ptr<CapEngine::EventDispatcher> eventDispatcher;
    CapEngine::Logger* logger;
    
    TopDownGame();
    void init();
    void start();
    void shutdown();

  private:
    bool quit;
    std::unique_ptr<CapEngine::Map2D> sp_map;
    CapEngine::Vector mapPosition;
    Hero hero;
    CapEngine::real speed;
    CapEngine::TimeStep timeStep;
    CapEngine::Keyboard keyboard;

    void mainLoop();
    void receiveEvent(const SDL_Event* event, CapEngine::Time* time);
    CapEngine::Rect calcMapDrawArea();
    void updateMovement();
    void render();
    void update();
  };
}

#endif //TOPDOWNGAME_H
