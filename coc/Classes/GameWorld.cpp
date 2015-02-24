#include "Base.h"
#include "GameWorld.h"
#include "MapManager.h"

GameWorld::~GameWorld()
{
    if (_mapManager != nullptr)
    {
        CC_SAFE_DELETE(_mapManager);
    }
}

bool GameWorld::init()
{
    if (!Node::init())
    {
        return false;
    }

    _mapManager = new MapManager();
    _mapManager->init(this, "isoMap.tmx");

    auto mouseListener = EventListenerMouse::create();
    mouseListener->onMouseScroll = CC_CALLBACK_1(GameWorld::onMouseScroll, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(mouseListener, this);

    scheduleUpdate();

    return true;
}

void GameWorld::update(float deltaTime)
{
    _mapManager->updateMapPosition();
}

void GameWorld::onMouseScroll(Event* event)
{
    _mapManager->updateMapScale(event);
}