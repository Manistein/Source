#pragma once

class GameObject;

enum class TileMapLayerType
{
    Invalid = 0,

    BackgroundLayer,
    GameObjcetLayer,

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

struct TileInfo
{
    int gid = 0;
    Vec2 centerPoint;
};

class MapManager
{
public:
    ~MapManager();

    bool init(Node* parentNode, const std::string& titleMapFileName);

    void updateMapPosition();
    void updateMapScale(Event* event);

    void syncCursorPoint(const Vec2& cursorPoint);

    Size getTileSize();
    Point getTileSubscript();
    DebugInfo getDebugInfo(TileMapLayerType tileMapLayerType);

    Vec2 convertCursorPositionToTileMapSpace();
    void addChildInGameObjectLayer(Node* gameObject, int zOrder = 1);

    TileInfo getTileInfoAt(int xSubscript, int ySubscript);
private:
    void resolveMapShakeWhenMove();

    void initTileMapInfoTable();

    cocos2d::experimental::TMXTiledMap* _tileMap = nullptr;
    float _mapScale = 0.5f;

    RECT _clientRect;
    Vec2 _cursorPoint;

    vector<vector<TileInfo>> _tileInfoTable;
};