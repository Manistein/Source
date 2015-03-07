#include "Base.h"
#include "GameWorld.h"
#include "MapManager.h"
#include "GameObject.h"
#include "Npc.h"

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

    _testNpc = static_cast<Npc*>(GameObjectFactory::create(GameObjectType::Npc, "BlueArcher", Vec2(2000.0f, 2000.0f)));
    _mapManager->addChildInGameObjectLayer(_testNpc);

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

void GameWorld::onMouseRightButtonDownEvent()
{
    auto targetPosition = _mapManager->convertCursorPositionToTileMapSpace();
    _testNpc->moveTo(targetPosition);
}
