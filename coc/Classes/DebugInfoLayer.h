#pragma once

class DebugInfoLayer : public Layer
{
public:
    CREATE_FUNC(DebugInfoLayer);

    void updateCusorPositionLabel(const Vec2& cusorPosition);
private:
    bool init() override;
    Label* createLabel(int fontSize, const Vec2& position, const std::string& text);

    Label* _cursorPositionLabel = nullptr;
    Label* _titleCoodinateLabel = nullptr;
    Label* _currentHoverTitleTextureNameLabel = nullptr;
    Label* _currentHoverTitleGIDLabel = nullptr;
    Label* _currentTitleMapLayerNameLabel = nullptr;
};