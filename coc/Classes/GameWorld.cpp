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
    _mapManager->init(this, "testMmap.tmx");

    scheduleUpdate();

    auto mouseListener = EventListenerMouse::create();
    mouseListener->onMouseMove = CC_CALLBACK_1(GameWorld::onMouseMove, this);
    mouseListener->onMouseUp = CC_CALLBACK_1(GameWorld::onMouseUp, this);
    mouseListener->onMouseDown = CC_CALLBACK_1(GameWorld::onMouseDown, this);
    mouseListener->onMouseScroll = CC_CALLBACK_1(GameWorld::onMouseScroll, this);

    _eventDispatcher->addEventListenerWithSceneGraphPriority(mouseListener, this);

    return true;
}

void GameWorld::update(float deltaTime)
{
    _mapManager->updateMapPosition();
}

void GameWorld::onMouseMove(Event* event)
{
    _mapManager->updateCursorPosition(event);
}

void GameWorld::onMouseDown(Event* event)
{

}

void GameWorld::onMouseUp(Event* event)
{

}

void GameWorld::onMouseScroll(Event* event)
{
    _mapManager->updateMapScale(event);
}

cocos2d::Vec2 GameWorld::getCursorPosition()
{
    return _mapManager->getCursorPosition();
}
