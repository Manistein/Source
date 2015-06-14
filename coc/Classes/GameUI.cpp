#include "Base.h"
#include "MapManager.h"
#include "GameUI.h"
#include "DebugInfoLayer.h"
#include "GameObject.h"
#include "GameWorld.h"
#include "GameWorldCallBackFunctionsManager.h"
#include "cocostudio/ActionTimeline/CSLoader.h"

bool GameUI::init()
{
    if (!Node::init())
    {
        return false;
    }

    _debugInfoLayer = DebugInfoLayer::create();
    addChild(_debugInfoLayer);

    _gameMainUI = CSLoader::createNode("MainScene.csb");
    addChild(_gameMainUI);

    Director::getInstance()->getEventDispatcher()->addCustomEventListener("MouseMove", CC_CALLBACK_1(GameUI::onMouseMove, this));

    scheduleUpdate();

    return true;
}

void GameUI::update(float deltaTime)
{
    auto& debugInfo = GameWorldCallBackFunctionsManager::getInstance()->_getDebugInfo();
    _debugInfoLayer->updateInfo(debugInfo);
}

void GameUI::syncCursorPoint(const Vec2& cursorPoint)
{
    _cursorPoint = cursorPoint;
}

void GameUI::onMouseMove(EventCustom* eventCustom)
{
    Vec2* cursorPoint = (Vec2*)eventCustom->getUserData();
    syncCursorPoint(*cursorPoint);
}

bool GameUI::isCursorInGameMainUI()
{
    bool result = false;

    auto gameMainPanel = _gameMainUI->getChildByName("Panel_GameMain");
    if (gameMainPanel->getBoundingBox().containsPoint(_cursorPoint))
    {
        result = true;
    }

    return result;
}
