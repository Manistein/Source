#include "Base.h"
#include "GameScene.h"
#include "GameWorld.h"
#include "GameUI.h"
#include "MapManager.h"

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

    _cursorPosition.x = mouseEvent->getCursorX();
    _cursorPosition.y = clientHeight + mouseEvent->getCursorY();

    _gameWorld->_mapManager->syncCursorPosition(_cursorPosition);
    _gameUI->syncCursorPosition(_cursorPosition);
}

void GameScene::onMouseDown(Event* event)
{

}

void GameScene::onMouseUp(Event* event)
{

}