#pragma once

struct DebugInfo;

class DebugInfoLayer : public Layer
{
public:
    CREATE_FUNC(DebugInfoLayer);

    void updateInfo(const DebugInfo& debugInfo);
private:
    bool init() override;
    Label* createLabel(int fontSize, const Vec2& position, const std::string& text);

    Label* _cursorPointLabel = nullptr;
    Label* _tileSubscriptLabel = nullptr;
    Label* _currentHoverTileTextureNameLabel = nullptr;
    Label* _currentHoverTileGIDLabel = nullptr;
    Label* _currentTileMapLayerNameLabel = nullptr;
    Label* _gameObjectCountLabel = nullptr;
};