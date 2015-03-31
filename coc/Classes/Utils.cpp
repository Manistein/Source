#include "Base.h"
#include "Utils.h"

float GameUtils::computeRotatedDegree(const Vec2& beginPosition, const Vec2& endPosition)
{
    float rotation = 0.0f;

    auto moveVector = endPosition - beginPosition;
    if (moveVector.x > -FLT_EPSILON && moveVector.x < FLT_EPSILON)
    {
        if (moveVector.y >= 0.0f)
        {
            rotation = 0.0f;
        }
        else
        {
            rotation = 180.0f;
        }

        return rotation;
    }
    else if (moveVector.y > -FLT_EPSILON && moveVector.y < FLT_EPSILON)
    {
        if (moveVector.x >= 0.0f)
        {
            rotation = 90.0f;
        }
        else
        {
            rotation = 270.0f;
        }

        return rotation;
    }

    float tanTheta = moveVector.x / moveVector.y;
    if (tanTheta > 0.0f)
    {
        if (moveVector.y > 0.0f)
        {
            rotation = atan(tanTheta) * 180.0f / M_PI;
        }
        else
        {
            rotation = atan(tanTheta) * 180.0f / M_PI + 180.0f;
        }
    }
    else
    {
        if (moveVector.x > 0.0f)
        {
            rotation = atan(tanTheta) * 180.0f / M_PI + 180.0f;
        }
        else
        {
            rotation = atan(tanTheta) * 180.0f / M_PI + 360.0f;
        }
    }

    return rotation;
}
