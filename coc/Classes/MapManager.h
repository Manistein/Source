#pragma once

enum class TileMapLayerType
{
    Invalid = 0,

    BackgroundLayer,
    GameObjcetLayer,
    RoadblockLayer,

    Total
};

struct DebugInfo
{
    Vec2 cursorPoint;
    Point tileSubscript;
    std::string tileTextureName;
    std::string tileMapLayerName;
    int gid = 0;
};

class MapManager
{
public:
    bool init(Node* parentNode, const std::string& titleMapFileName);

    void updateMapPosition();
    void updateMapScale(Event* event);

    void syncCursorPoint(const Vec2& cursorPoint);

    Point getTileSubscript();
    DebugInfo getDebugInfo(TileMapLayerType tileMapLayerType);
private:
    void resolveMapShakeWhenMove();

    TMXTiledMap* _tileMap = nullptr;
    float _mapScale = 0.5f;

    RECT _clientRect;
    Vec2 _cursorPoint;
};