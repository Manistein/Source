#include "Base.h"
#include "MapManager.h"
#include "DebugInfoLayer.h"
#include "GameObject.h"
#include "GameWorld.h"


bool DebugInfoLayer::init()
{
    if (!Layer::init())
    {
        return false;
    }

    _cursorPointLabel = createLabel(20, Vec2(10.0f, 640.0f), "");
    _tileSubscriptLabel = createLabel(20, Vec2(10.0f, 620.0f), "");
    _currentTileMapLayerNameLabel = createLabel(20, Vec2(10.0f, 600.0f), "");
    _currentHoverTileTextureNameLabel = createLabel(20, Vec2(10.0f, 580.0f), "");
    _currentHoverTileGIDLabel = createLabel(20, Vec2(10.0f, 560.0f), "");
    _gameObjectCountLabel = createLabel(20, Vec2(10.0f, 540.0f), "");

    return true;
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

void DebugInfoLayer::updateInfo(const DebugInfo& debugInfo)
{
    auto cursorXString = StringUtils::format("%.2f", debugInfo.mapDebugInfo.cursorPoint.x);
    auto cursorYString = StringUtils::format("%.2f", debugInfo.mapDebugInfo.cursorPoint.y);
    _cursorPointLabel->setString("CX = " + cursorXString + ", CY = " + cursorYString);

    auto tileXSubscriptString = StringUtils::format("%.2f", debugInfo.mapDebugInfo.tileSubscript.x);
    auto tileYSubscriptString = StringUtils::format("%.2f", debugInfo.mapDebugInfo.tileSubscript.y);
    _tileSubscriptLabel->setString("TileXSubscript = " + tileXSubscriptString + ", TileYSubscript = " + tileYSubscriptString);

    _currentTileMapLayerNameLabel->setString("TileMapLayerName = " + debugInfo.mapDebugInfo.tileMapLayerName);
    _currentHoverTileTextureNameLabel->setString("TileTextureName = " + debugInfo.mapDebugInfo.tileTextureName);
    _currentHoverTileGIDLabel->setString("Tile gid = " + StringUtils::format("%d", debugInfo.mapDebugInfo.gid));

    _gameObjectCountLabel->setString("GameObject count = " + StringUtils::format("%d", debugInfo.gameObjectCount));
}
