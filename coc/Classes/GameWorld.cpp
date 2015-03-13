#include "Base.h"
#include "GameObject.h"
#include "MapManager.h"
#include "GameObjectManager.h"
#include "GameObjectSelectBox.h"
#include "Npc.h"
#include "GameWorld.h"

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

    _gameObjectManager = GameObjectManager::getInstance();

    _gameObjectSelectBox = GameObjectSelectBox::create();
    addChild(_gameObjectSelectBox);

    auto director = Director::getInstance();
    director->getEventDispatcher()->addCustomEventListener("MouseLeftButtonDown", CC_CALLBACK_0(GameWorld::onMouseLeftButtonDown, this));
    director->getEventDispatcher()->addCustomEventListener("MouseLeftButtonUp", CC_CALLBACK_0(GameWorld::onMouseLeftButtonUp, this));
    director->getEventDispatcher()->addCustomEventListener("MouseRightButtonDown", CC_CALLBACK_0(GameWorld::onMouseRightButtonDown, this));
    director->getEventDispatcher()->addCustomEventListener("MouseRightButtonUp", CC_CALLBACK_0(GameWorld::onMouseRightButtonUp, this));
    director->getEventDispatcher()->addCustomEventListener("MouseMove", CC_CALLBACK_1(GameWorld::onMouseMove, this));

    createGameObject(GameObjectType::Npc, "BlueArcher", Vec2(2000.0f, 2000.0f));

    scheduleUpdate();

    return true;
}

void GameWorld::update(float deltaTime)
{
    _mapManager->updateMapPosition();
    _gameObjectManager->gameObjectsDepthSort(_mapManager->getTileSize());
}

void GameWorld::onMouseScroll(Event* event)
{
    _mapManager->updateMapScale(event);
}

void GameWorld::onMouseLeftButtonDown()
{
    _gameObjectSelectBox->setMouseDownStatus(true);
    _gameObjectSelectBox->setMouseDownPoint(_cursorPoint);
}

void GameWorld::onMouseLeftButtonUp()
{
    _gameObjectSelectBox->setMouseDownStatus(false);
}

void GameWorld::onMouseRightButtonDown()
{
    auto targetPosition = _mapManager->convertCursorPositionToTileMapSpace();
}

void GameWorld::onMouseRightButtonUp()
{

}

void GameWorld::onMouseMove(EventCustom* eventCustom)
{
    Vec2* cursorPointVec = (Vec2*)eventCustom->getUserData();
    syncCursorPoint(*cursorPointVec);
}

void GameWorld::createGameObject(GameObjectType type, const string& jobName, const Vec2& position)
{
    auto gameObject = _gameObjectManager->createGameObject(type, jobName, position);
    _mapManager->addChildInGameObjectLayer(gameObject);
}

void GameWorld::removeGameObjectBy(int uniqueID)
{
    _gameObjectManager->removeGameObjectBy(uniqueID);
}

void GameWorld::syncCursorPoint(const Vec2& cursorPoint)
{
    _cursorPoint = cursorPoint;
    _mapManager->syncCursorPoint(cursorPoint);
    _gameObjectSelectBox->syncCursorPoint(cursorPoint);
}