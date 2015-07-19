#pragma once

namespace GameUtils
{
    float computeRotatedDegree(const Vec2& beginPosition, const Vec2& endPosition);
    float computeDistanceBetween(const Vec2& beginPosition, const Vec2& endPosition);
    bool isFloatEqual(float left, float right);
    bool isVec2Equal(const Vec2& left, const Vec2& right);
    Animation* createAnimationWithPList(const string& plistFileName);
    void pauseTargetAndChildren(Node* target);
    bool convertEncoding(const string& toCode, const string& fromCode, const string& fromCodeText, string& toCodeText);
}