#include "Base.h"
#include "GameScene.h"
#include "GameWorld.h"
#include "GameUI.h"

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

    scheduleUpdate();

    return true;
}

void GameScene::update(float deltaTime)
{
    auto cursorPosition = _gameWorld->getCursorPosition();
    _gameUI->setCursorPosition(cursorPosition);
}

void GameScene::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event)
{
    if (keyCode == EventKeyboard::KeyCode::KEY_ESCAPE)
    {
        Director::getInstance()->end();
    }
}
