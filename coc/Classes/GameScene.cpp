#include "Base.h"
#include "MapManager.h"
#include "DebugInfoLayer.h"
#include "GameObject.h"
#include "TemplatesManager.h"
#include "GameUI.h"
#include "GameWorld.h"
#include "GameScene.h"
#include "GameSetting.h"
#include "audio/include/AudioEngine.h"
#include "SoundManager.h"
#include "WindowsHelper.h"

const int MOUSE_LEFT_BUTTON = 0;
const int MOUSE_RIGHT_BUTTON = 1;

GameSetting g_setting;

const static unordered_map<EventKeyboard::KeyCode, int> s_keyCodeToNumberMap = {
        { EventKeyboard::KeyCode::KEY_0, 0 },
        { EventKeyboard::KeyCode::KEY_1, 1 },
        { EventKeyboard::KeyCode::KEY_2, 2 },
        { EventKeyboard::KeyCode::KEY_3, 3 },
        { EventKeyboard::KeyCode::KEY_4, 4 },
        { EventKeyboard::KeyCode::KEY_5, 5 },
        { EventKeyboard::KeyCode::KEY_6, 6 },
        { EventKeyboard::KeyCode::KEY_7, 7 },
        { EventKeyboard::KeyCode::KEY_8, 8 },
        { EventKeyboard::KeyCode::KEY_9, 9 },
};

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

    srand(::timeGetTime());

    // initNpcResources();

    SoundManager::getInstance();
    WindowsHelper::getInstance()->switchToNormalCursor();
    _director = Director::getInstance();

    _gameWorld = GameWorld::create();
    addChild(_gameWorld);

    _gameUI = GameUI::create();
    addChild(_gameUI);

    auto glView = Director::getInstance()->getOpenGLView();
    auto windowHandle = glView->getWin32Window();
    ::GetClientRect(windowHandle, &_clientRect);

    auto keyboardListener = EventListenerKeyboard::create();
    keyboardListener->onKeyReleased = CC_CALLBACK_2(GameScene::onKeyReleased, this);
    keyboardListener->onKeyPressed = CC_CALLBACK_2(GameScene::onKeyPressed, this);
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

void GameScene::onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event)
{
    if (keyCode == EventKeyboard::KeyCode::KEY_CTRL)
    {
        _gameWorld->setCtrlKeyStatus(true);
    }
    else if (keyCode == EventKeyboard::KeyCode::KEY_SHIFT)
    {
        _gameWorld->setShiftKeyStatus(true);
    }
    else if (keyCode == EventKeyboard::KeyCode::KEY_A)
    {
        _gameWorld->sendMopUpCommandForPlayerForce();
    }
}

void GameScene::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event)
{
    switch (keyCode)
    {
    case EventKeyboard::KeyCode::KEY_F5:
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
        break;
    case EventKeyboard::KeyCode::KEY_0:
    case EventKeyboard::KeyCode::KEY_1:
    case EventKeyboard::KeyCode::KEY_2:
    case EventKeyboard::KeyCode::KEY_3:
    case EventKeyboard::KeyCode::KEY_4:
    case EventKeyboard::KeyCode::KEY_5:
    case EventKeyboard::KeyCode::KEY_6:
    case EventKeyboard::KeyCode::KEY_7:
    case EventKeyboard::KeyCode::KEY_8:
    case EventKeyboard::KeyCode::KEY_9:
    {
        auto iter = s_keyCodeToNumberMap.find(keyCode);
        int teamID = iter->second;

        _gameWorld->onPlayerManipulateTeamBy(teamID);
    }
        break;
    case EventKeyboard::KeyCode::KEY_CTRL:
    {
        _gameWorld->setCtrlKeyStatus(false);
    }
        break;
    case EventKeyboard::KeyCode::KEY_SHIFT:
    {
        _gameWorld->setShiftKeyStatus(false);
    }
        break;
    case EventKeyboard::KeyCode::KEY_SPACE:
    {
        _gameWorld->onJumpToPlayerBaseCamp();
    }
        break;
    case EventKeyboard::KeyCode::KEY_F10:
    {
        if (_gameUI->isShowDebugLayer())
        {
            _gameUI->setDebugLayerShowStatus(false);
            Director::getInstance()->setDisplayStats(false);
        }
        else
        {
            _gameUI->setDebugLayerShowStatus(true);
            Director::getInstance()->setDisplayStats(true);
        }
    }
        break;
    default:    break;
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
    if (_gameUI->isCursorInGameMainUI())
    {
        return;
    }

    auto mouseEvent = static_cast<EventMouse*>(event);
    if (mouseEvent->getMouseButton() == MOUSE_LEFT_BUTTON)
    {
        _director->getEventDispatcher()->dispatchCustomEvent("GameWorldMouseLeftButtonDownEvent");
    }
    else
    {
        _director->getEventDispatcher()->dispatchCustomEvent("GameWorldMouseRightButtonDownEvent");
    }
}

void GameScene::onMouseUp(Event* event)
{
    // 如果玩家鼠标是拖动状态的话，仍然可以触发mouseUp事件
    if (_gameUI->isCursorInGameMainUI() &&
        _gameWorld->isMouseClick())
    {
        return;
    }

    auto mouseEvent = static_cast<EventMouse*>(event);
    if (mouseEvent->getMouseButton() == MOUSE_LEFT_BUTTON)
    {
        _director->getEventDispatcher()->dispatchCustomEvent("GameWorldMouseLeftButtonUpEvent");
    }
    else
    {
        _director->getEventDispatcher()->dispatchCustomEvent("GameWorldMouseRightButtonUpEvent");
    }
}

void GameScene::onExitGame()
{
    WindowsHelper::getInstance()->uninitInstane();
    Director::getInstance()->end();
}
