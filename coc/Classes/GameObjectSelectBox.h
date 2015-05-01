#pragma once

class GameObjectSelectBox : public DrawNode
{
public:
    CREATE_FUNC(GameObjectSelectBox);

    void setMouseDownStatus(bool isMouseDown);
    void setMouseDownPoint(const Vec2& point);

    void syncCursorPoint(const Vec2& point);
    void update(float delta) override;
    Rect getRect();
private:
    bool init() override;

    bool _isMouseDown = false;
    Vec2 _mouseDownPoint;
    Vec2 _cursorPoint;

    Color4F _color;
    Size _boxSize;
    Vec2 _leftBottomPoint;
    Vec2 _rightTopPoint;
    Rect _rect;
};