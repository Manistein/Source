#include "Base.h"
#include "GameUI.h"
#include "DebugInfoLayer.h"

bool GameUI::init()
{
    if (!Node::init())
    {
        return false;
    }

    _debugInfoLayer = DebugInfoLayer::create();
    addChild(_debugInfoLayer);

    scheduleUpdate();

    return true;
}

void GameUI::update(float deltaTime)
{

}

void GameUI::syncCursorPoint(const Vec2& cursorPoint)
{
    _cursorPoint = cursorPoint;
}
