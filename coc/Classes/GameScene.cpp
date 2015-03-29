#include "Base.h"
#include "MapManager.h"
#include "DebugInfoLayer.h"
#include "GameObject.h"
#include "TemplatesManager.h"
#include "GameUI.h"
#include "GameWorld.h"
#include "GameScene.h"
#include "GameSetting.h"

const int MOUSE_LEFT_BUTTON = 0;
const int MOUSE_RIGHT_BUTTON = 1;

GameSetting g_setting;

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

    TemplateManager::getInstance();
    _director = Director::getInstance();

    _gameWorld = GameWorld::create();
    addChild(_gameWorld);

    _gameUI = GameUI::create();
    addChild(_gameUI);

    _gameUI->registerGameWorldCallBackFunctions(_gameWorld);

    auto glView = Director::getInstance()->getOpenGLView();
    auto windowHandle = glView->getWin32Window();
    ::GetClientRect(windowHandle, &_clientRect);

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
    else if (keyCode == EventKeyboard::KeyCode::KEY_F5)
    {
        if (g_setting.allowDebugDraw)
        {
            g_setting.allowDebugDraw = false;
            _director->getEventDispatcher()->dispatchCustomEvent("ClearDebugDraw");
        }
        else
        {
            g_setting.allowDebugDraw = true;
        }
    }
}

void GameScene::onMouseMove(Event* event)
{
    auto mouseEvent = static_cast<EventMouse*>(event);

    auto clientHeight = _clientRect.bottom - _clientRect.top;

    _cursorPoint.x = mouseEvent->getCursorX();
    _cursorPoint.y = clientHeight + mouseEvent->getCursorY();

    _director->getEventDispatcher()->dispatchCustomEvent("MouseMove", (void*)&_cursorPoint);
}

void GameScene::onMouseDown(Event* event)
{
    auto mouseEvent = static_cast<EventMouse*>(event);
    if (mouseEvent->getMouseButton() == MOUSE_LEFT_BUTTON)
    {
        _director->getEventDispatcher()->dispatchCustomEvent("MouseLeftButtonDown");
    }
    else
    {
        _director->getEventDispatcher()->dispatchCustomEvent("MouseRightButtonDown");
    }
}

void GameScene::onMouseUp(Event* event)
{
    auto mouseEvent = static_cast<EventMouse*>(event);
    if (mouseEvent->getMouseButton() == MOUSE_LEFT_BUTTON)
    {
        _director->getEventDispatcher()->dispatchCustomEvent("MouseLeftButtonUp");
    }
    else
    {
        _director->getEventDispatcher()->dispatchCustomEvent("MouseRightButtonUp");
    }
}
