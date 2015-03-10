#include "Base.h"
#include "GameObjectSelectBox.h"

void GameObjectSelectBox::setMouseDownStatus(bool isMouseDown)
{
    _isMouseDown = isMouseDown;
}

void GameObjectSelectBox::setMouseDownPoint(const Vec2& point)
{
    _mouseDownPoint = point;
}

void GameObjectSelectBox::setMouseMovePoint(const Vec2& point)
{
    _mouseMovePoint = point;
}

void GameObjectSelectBox::update(float delta)
{
    clear();

    if (_isMouseDown)
    {
        _leftBottomPoint.x = std::min(_mouseDownPoint.x, _mouseMovePoint.x);
        _leftBottomPoint.y = std::min(_mouseDownPoint.y, _mouseMovePoint.y);

        _boxSize.width = std::abs(_mouseDownPoint.x - _mouseMovePoint.x);
        _boxSize.height = std::abs(_mouseDownPoint.y - _mouseMovePoint.y);

        _rightTopPoint.x = _leftBottomPoint.x + _boxSize.width;
        _rightTopPoint.y = _leftBottomPoint.y + _boxSize.height;

        drawSolidRect(_leftBottomPoint, _rightTopPoint, _color);
    }
}

bool GameObjectSelectBox::init()
{
    if (!DrawNode::init())
    {
        return false;
    }

    _color = Color4F(0.0f, 0.0f, 1.0f, 0.2f);

    scheduleUpdate();

    return true;
}
