#ifndef TOPDOWNGAME_H
#define TOPDOWNGAME_H

#include <memory>

#include "CapEngine.h"

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
    CapEngine::real speed;
    CapEngine::TimeStep timeStep;

    void mainLoop();
    void receiveEvent(const SDL_Event* event, CapEngine::Time* time);
    CapEngine::Rect calcMapDrawArea();

  };
}

#endif //TOPDOWNGAME_H
