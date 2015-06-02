#include "Base.h"
#include "MapManager.h"
#include "GameUI.h"
#include "DebugInfoLayer.h"
#include "GameObject.h"
#include "GameWorld.h"
#include "GameWorldCallBackFunctionsManager.h"

bool GameUI::init()
{
    if (!Node::init())
    {
        return false;
    }

    _debugInfoLayer = DebugInfoLayer::create();
    addChild(_debugInfoLayer);

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
