#include "Base.h"
#include "GameScene.h"
#include "GameWorld.h"
#include "GameUI.h"
#include "MapManager.h"
#include "DebugInfoLayer.h"

Scene* GameScene::createScene()
{
    auto scene = Scene::create();
    
    auto layer = GameScene::create();

    scene->addChild(layer);

    return scene;
}

bool GameScene::init()
{
    if ( !Layer::init() )
    {
        return false;
    }

    _gameWorld = GameWorld::create();
    addChild(_gameWorld);

    _gameUI = GameUI::create();
    addChild(_gameUI);

    auto keyboardListener = EventListenerKeyboard::create();
    keyboardListener->onKeyReleased = CC_CALLBACK_2(GameScene::onKeyReleased, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(keyboardListener, this);

    auto mouseListener = EventListenerMouse::create();
    mouseListener->onMouseMove = CC_CALLBACK_1(GameScene::onMouseMove, this);
    mouseListener->onMouseUp = CC_CALLBACK_1(GameScene::onMouseUp, this);
    mouseListener->onMouseDown = CC_CALLBACK_1(GameScene::onMouseDown, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(mouseListener, this);

    scheduleUpdate();

    return true;
}

void GameScene::update(float deltaTime)
{
    updateDebugInfoLayer();
}

void GameScene::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event)
{
    if (keyCode == EventKeyboard::KeyCode::KEY_ESCAPE)
    {
        Director::getInstance()->end();
    }
}

void GameScene::onMouseMove(Event* event)
{
    auto mouseEvent = static_cast<EventMouse*>(event);
    auto clientHeight = Director::getInstance()->getVisibleSize().height;

    _cursorPoint.x = mouseEvent->getCursorX();
    _cursorPoint.y = clientHeight + mouseEvent->getCursorY();

    _gameWorld->_mapManager->syncCursorPoint(_cursorPoint);
    _gameUI->syncCursorPoint(_cursorPoint);
}

void GameScene::onMouseDown(Event* event)
{

}

void GameScene::onMouseUp(Event* event)
{

}

void GameScene::updateDebugInfoLayer()
{
    if (_gameUI->_debugInfoLayer->isVisible())
    {
        auto debugInfo = _gameWorld->_mapManager->getDebugInfo(TileMapLayerType::RoadblockLayer);
        _gameUI->_debugInfoLayer->updateInfo(debugInfo);
    }
}
