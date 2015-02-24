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

void GameUI::syncCursorPosition(const Vec2& cursorPosition)
{
    _cursorPosition = cursorPosition;

    _debugInfoLayer->updateCusorPositionLabel(_cursorPosition);
}
