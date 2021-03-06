#ifndef TOPDOWNGAME_H
#define TOPDOWNGAME_H

#include <memory>

#include "CapEngine.h"
#include "hero.h"

namespace TopDown{

  class TopDownGame : public IEventSubscriber {
  public:
    TopDownGame();
    void init();
    void start();
    void shutdown();

  private:
    void mainLoop();
    void receiveEvent(const SDL_Event* event, CapEngine::Time* time);
    CapEngine::Rect calcMapDrawArea();
    void updateMovement();
    void render();
    void update();

  public:
    struct CapEngine::Screen_t screenConfig;
    CapEngine::VideoManager* vManager;
    CapEngine::EventDispatcher* eventDispatcher;
    CapEngine::Logger* logger;

  private:
    bool quit;
    std::unique_ptr<CapEngine::Map2D> sp_map;
    CapEngine::Vector mapPosition;
    Hero hero;
    CapEngine::real speed;
    CapEngine::TimeStep timeStep;
    CapEngine::Keyboard keyboard;

  };
}

#endif //TOPDOWNGAME_H
