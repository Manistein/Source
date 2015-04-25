#include "Base.h"
#include "GameObject.h"
#include "MapManager.h"
#include "GameObjectManager.h"
#include "GameObjectSelectBox.h"
#include "Npc.h"
#include "GameWorld.h"
#include "BulletManager.h"
#include "GameWorldCallBackFunctionsManager.h"
#include "AutoFindPathHelper.h"

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
    _gameObjectSelectBox->setGlobalZOrder(MAX_GAME_OBJECT_COUNT);
    addChild(_gameObjectSelectBox);

    _bulletManager = BulletManager::getInstance();

    GameWorldCallBackFunctionsManager::getInstance()->registerCallBackFunctions(this);

    auto mouseListener = EventListenerMouse::create();
    mouseListener->onMouseScroll = CC_CALLBACK_1(GameWorld::onMouseScroll, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(mouseListener, this);

    auto director = Director::getInstance();
    director->getEventDispatcher()->addCustomEventListener("MouseLeftButtonDown", CC_CALLBACK_0(GameWorld::onMouseLeftButtonDown, this));
    director->getEventDispatcher()->addCustomEventListener("MouseLeftButtonUp", CC_CALLBACK_0(GameWorld::onMouseLeftButtonUp, this));
    director->getEventDispatcher()->addCustomEventListener("MouseRightButtonDown", CC_CALLBACK_0(GameWorld::onMouseRightButtonDown, this));
    director->getEventDispatcher()->addCustomEventListener("MouseRightButtonUp", CC_CALLBACK_0(GameWorld::onMouseRightButtonUp, this));
    director->getEventDispatcher()->addCustomEventListener("MouseMove", CC_CALLBACK_1(GameWorld::onMouseMove, this));
    director->getEventDispatcher()->addCustomEventListener("ClearDebugDraw", CC_CALLBACK_0(GameWorld::onClearDebugDraw, this));


    createGameObject(GameObjectType::Npc, ForceType::Player, "BlueArcher", Vec2(2000.0f, 2000.0f));
    createGameObject(GameObjectType::Npc, ForceType::Player, "BlueArcher", Vec2(2050.0f, 2050.0f));
    createGameObject(GameObjectType::Npc, ForceType::Player, "BlueArcher", Vec2(2100.0f, 2100.0f));

    createGameObject(GameObjectType::Npc, ForceType::AI, "PinkArcher", Vec2(3150.0f, 2150.0f));

    _director->setAlphaBlending(true);

    srand(::timeGetTime());

    scheduleUpdate();

    return true;
}

void GameWorld::update(float deltaTime)
{
    _mapManager->updateMapPosition();
    _gameObjectManager->gameObjectsDepthSort(_mapManager->getTileSize());

    _gameObjectManager->removeAllDecimatedGameObjects();
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
    _gameObjectManager->cancelAllGameObjectSelected();

    if (std::abs(_cursorPoint.x - s_mouseDownPoint.x) < SINGLE_CLICK_AREA && std::abs(_cursorPoint.y - s_mouseDownPoint.y) < SINGLE_CLICK_AREA)
    {
        _gameObjectManager->selectGameObjectBy(_cursorPoint);
    }
    else
    {
        auto selectRect = _gameObjectSelectBox->getRect();
        _gameObjectManager->selectGameObjectsBy(selectRect);
    }
}

void GameWorld::onMouseRightButtonDown()
{
    auto inMapCursorPosition = _mapManager->convertCursorPositionToTileMapSpace();
    if (_mapManager->isInObstacleTile(inMapCursorPosition))
    {
        return;
    }

    auto gameObjectSelectedByPlayerCount = _gameObjectManager->getGameObjectSelectedByPlayerCount();

    if (gameObjectSelectedByPlayerCount == 1)
    {
        _gameObjectManager->npcSelectedByPlayerMoveTo(inMapCursorPosition);
    }
    else
    {
        auto npcMoveEndPositionList = _mapManager->getNpcMoveEndPositionListBy(gameObjectSelectedByPlayerCount);
        _gameObjectManager->npcSelectedByPlayerMoveTo(npcMoveEndPositionList);
    }

    auto enemy = _gameObjectManager->selectEnemyBy(_cursorPoint);
    if (enemy)
    {
        _gameObjectManager->setSelectedGameObjectEnemyUniqueID(enemy->getUniqueID());
    }
    else
    {
        _gameObjectManager->setSelectedGameObjectEnemyUniqueID(ENEMY_UNIQUE_ID_INVALID);
        _gameObjectManager->cancelEnemySelected();
    }
}

void GameWorld::onClearDebugDraw()
{
    _gameObjectManager->clearGameObjectDebugDraw();
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

void GameWorld::createBullet(BulletType bulletType, int attackerID, int attackTargetID)
{
    auto bullet = _bulletManager->createBullet(bulletType, attackerID, attackTargetID);
    if (bullet)
    {
        _mapManager->addChildInGameObjectLayer(bullet);
    }
}

void GameWorld::syncCursorPoint(const Vec2& cursorPoint)
{
    _cursorPoint = cursorPoint;
    _mapManager->syncCursorPoint(cursorPoint);
    _gameObjectSelectBox->syncCursorPoint(cursorPoint);
}

list<Vec2> GameWorld::computePathListBetween(const Vec2& inMapStartPosition, const Vec2& inMapEndPosition)
{
    list<Vec2> pointPathList;

    auto startTileSubscript = _mapManager->getTileSubscript(inMapStartPosition);
    auto startTileNode = _mapManager->getTileNodeAt((int)startTileSubscript.x, (int)startTileSubscript.y);
    auto distanceBetweenTileAndNpc = inMapStartPosition - startTileNode->leftTopPosition;

    auto endTileSubscript = _mapManager->getTileSubscript(inMapEndPosition);
    auto endTileNode = _mapManager->getTileNodeAt((int)endTileSubscript.x, (int)endTileSubscript.y);

    auto tileNodePathList = AutoFindPathHelper::computeTileNodePathListBetween(startTileNode, endTileNode);

    for (auto tileNodePath: tileNodePathList)
    {
        pointPathList.push_front(tileNodePath->leftTopPosition + distanceBetweenTileAndNpc);
    }

    return pointPathList;
}
