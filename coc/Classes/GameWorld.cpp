#include "Base.h"
#include "GameObject.h"
#include "MapManager.h"
#include "GameObjectManager.h"
#include "GameObjectSelectBox.h"
#include "Npc.h"
#include "GameWorld.h"

static Vec2 s_mouseDownPoint;
const float SINGLE_CLICK_AREA = 5.0f;

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

    _gameObjectManager = GameObjectManager::getInstance();
    _gameObjectManager->init(this);

    _gameObjectSelectBox = GameObjectSelectBox::create();
    addChild(_gameObjectSelectBox);

    auto mouseListener = EventListenerMouse::create();
    mouseListener->onMouseScroll = CC_CALLBACK_1(GameWorld::onMouseScroll, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(mouseListener, this);

    auto director = Director::getInstance();
    director->getEventDispatcher()->addCustomEventListener("MouseLeftButtonDown", CC_CALLBACK_0(GameWorld::onMouseLeftButtonDown, this));
    director->getEventDispatcher()->addCustomEventListener("MouseLeftButtonUp", CC_CALLBACK_0(GameWorld::onMouseLeftButtonUp, this));
    director->getEventDispatcher()->addCustomEventListener("MouseRightButtonDown", CC_CALLBACK_0(GameWorld::onMouseRightButtonDown, this));
    director->getEventDispatcher()->addCustomEventListener("MouseRightButtonUp", CC_CALLBACK_0(GameWorld::onMouseRightButtonUp, this));
    director->getEventDispatcher()->addCustomEventListener("MouseMove", CC_CALLBACK_1(GameWorld::onMouseMove, this));

    createGameObject(GameObjectType::Npc, ForceType::Player, "BlueArcher", Vec2(2000.0f, 2000.0f));

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

    s_mouseDownPoint = _cursorPoint;
}

void GameWorld::onMouseLeftButtonUp()
{
    _gameObjectSelectBox->setMouseDownStatus(false);

    if (std::abs(_cursorPoint.x - s_mouseDownPoint.x) < SINGLE_CLICK_AREA && std::abs(_cursorPoint.y - s_mouseDownPoint.y) < SINGLE_CLICK_AREA)
    {
        if (!_gameObjectManager->selectGameObjectsBy(_cursorPoint))
        {
            _gameObjectManager->cancelSelected();
        }
    }
    else
    {
        auto selectRect = _gameObjectSelectBox->getRect();

        if (!_gameObjectManager->selectGameObjectsBy(selectRect))
        {
            _gameObjectManager->cancelSelected();
        }
    }
}

void GameWorld::onMouseRightButtonDown()
{
    auto targetPosition = _mapManager->convertCursorPositionToTileMapSpace();
    _gameObjectManager->selectedNpcMoveTo(targetPosition);
}

void GameWorld::onMouseRightButtonUp()
{

}

void GameWorld::onMouseMove(EventCustom* eventCustom)
{
    Vec2* cursorPointVec = (Vec2*)eventCustom->getUserData();
    syncCursorPoint(*cursorPointVec);
}

void GameWorld::createGameObject(GameObjectType gameObjectType, ForceType forceType, const string& jobName, const Vec2& position)
{
    auto gameObject = _gameObjectManager->createGameObject(gameObjectType, forceType, jobName, position);
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