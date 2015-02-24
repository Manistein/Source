#include "Base.h"
#include "DebugInfoLayer.h"


bool DebugInfoLayer::init()
{
    if (!Layer::init())
    {
        return false;
    }

    _cursorPositionLabel = createLabel(20, Vec2(10.0f, 690.0f), "");

    return true;
}

void DebugInfoLayer::updateCusorPositionLabel(const Vec2& cusorPosition)
{
    auto xPositionString = StringUtils::format("%.2f", cusorPosition.x);
    auto yPositionString = StringUtils::format("%.2f", cusorPosition.y);

    _cursorPositionLabel->setString("CX = " + xPositionString + ", CY = " + yPositionString);
}

Label* DebugInfoLayer::createLabel(int fontSize, const Vec2& position, const std::string& text)
{
    TTFConfig ttfConfig("arial.ttf", fontSize);
    auto label = Label::createWithTTF(ttfConfig, "");
    label->setPosition(position);
    label->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
    addChild(label);

    return label;
}
