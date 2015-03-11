#include "Base.h"
#include "MapManager.h"
#include "Npc.h"
#include "GameObjectManager.h"
#include "GameWorld.h"
#include "GameObjectSelectBox.h"

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

    //createGameObject(GameObjectType::Npc, "BlueArcher", Vec2(2000.0f, 2000.0f));

    _testNpc = static_cast<Npc*>(GameObjectFactory::create(GameObjectType::Npc, "BlueArcher", Vec2(2000.0f, 2000.0f)));
    _mapManager->addChildInGameObjectLayer(_testNpc);

    auto director = Director::getInstance();
    director->getEventDispatcher()->addCustomEventListener("MouseButtonDown", CC_CALLBACK_0(GameWorld::onMouseButtonDown, this));
    director->getEventDispatcher()->addCustomEventListener("MouseButtonUp", CC_CALLBACK_0(GameWorld::onMouseButtonUp, this));

    scheduleUpdate();

    return true;
}

void GameWorld::update(float deltaTime)
{
    _mapManager->updateMapPosition();

    _testNpc->depthSort(_mapManager->getTileSize());
}

void GameWorld::onMouseScroll(Event* event)
{
    _mapManager->updateMapScale(event);
}

void GameWorld::onMouseButtonDown()
{
    auto targetPosition = _mapManager->convertCursorPositionToTileMapSpace();
    _testNpc->moveTo(targetPosition);

    _gameObjectSelectBox->setMouseDownStatus(true);
    _gameObjectSelectBox->setMouseDownPoint(_cursorPoint);
}

void GameWorld::onMouseButtonUp()
{
    _gameObjectSelectBox->setMouseDownStatus(false);
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
    _gameObjectSelectBox->setMouseMovePoint(_cursorPoint);
}