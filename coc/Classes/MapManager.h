#pragma once

class MapManager
{
public:
    bool init(Node* parentNode, const std::string& titleMapFileName);

    void updateMapPosition();
    void updateMapScale(Event* event);

    void syncCursorPosition(const Vec2& cursorPosition);
private:
    void resolveMapShakeWhenMove();

    TMXTiledMap* _titleMap = nullptr;
    float _mapScale = 0.5f;

    RECT _clientRect;
    Vec2 _cursorPosition;
};