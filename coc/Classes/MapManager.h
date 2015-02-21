#pragma once

class MapManager
{
public:
    bool init(Node* parentNode, const std::string& titleMapFileName);

    void updateMapPosition();
    void updateMapScale(Event* event);
    void updateCursorPosition(Event* event);

    Vec2 getCursorPosition();
private:
    TMXTiledMap* _titleMap = nullptr;
    float _mapScale = 0.7f;

    RECT _clientRect;
    Vec2 _cursorPosition;
};