#include "Base.h"
#include "CustomMoveBy.h"


CustomMoveBy* CustomMoveBy::create(float duration, const Vec2& deltaPosition)
{
    CustomMoveBy *ret = new (std::nothrow) CustomMoveBy();

    if (ret)
    {
        if (ret->initWithDuration(duration, Vec3(deltaPosition.x, deltaPosition.y, 0.0f)))
        {
            ret->autorelease();
        }
        else
        {
            delete ret;
            ret = nullptr;
        }
    }

    return ret;
}

void CustomMoveBy::update(float executePercent)
{
    if (_target)
    {
        float delta = executePercent - _previousExecutePercent;

        auto selfPosition = _target->getPosition();
        selfPosition.x += _positionDelta.x * delta;
        selfPosition.y += _positionDelta.y * delta;

        _target->setPosition(selfPosition);

        _previousExecutePercent = executePercent;
    }
}
