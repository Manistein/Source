#include "Base.h"
#include "GameUI.h"

bool GameUI::init()
{
    if (!Node::init())
    {
        return false;
    }

    _cursorPositionLabel = Label::create();
    _cursorPositionLabel->setPosition(Vec2(100.0f, 600.0f));
    addChild(_cursorPositionLabel);

    scheduleUpdate();

    return true;
}

void GameUI::update(float deltaTime)
{
    updateCursorPosition();
}

void GameUI::updateCursorPosition()
{
    auto xPositionString = StringUtils::format("%f", _cursorPosition.x);
    auto yPositionString = StringUtils::format("%f", _cursorPosition.y);

    _cursorPositionLabel->setString("X = " + xPositionString + ", Y = " + yPositionString);
}

void GameUI::setCursorPosition(const Vec2& cursorPosition)
{
    _cursorPosition = cursorPosition;
}
