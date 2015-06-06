#pragma once

class CustomMoveBy : public MoveBy
{
public:
    static CustomMoveBy* create(float duration, const Vec2& deltaPosition);
    void update(float executePercent) override;

private:
    float _previousExecutePercent = 0.0f;
};