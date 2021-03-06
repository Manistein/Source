#pragma once

class GameObject;

enum class TileMapLayerType
{
    Invalid = 0,

    BackgroundLayer,
    GameObjcetLayer,

    Total
};

struct MapDebugInfo
{
    Vec2 cursorPoint;
    Point tileSubscript;
    std::string tileTextureName;
    std::string tileMapLayerName;
    int gid = 0;
};

struct TileNode
{
    int gid = 0;
    Vec2 leftTopPosition;

    int gotoEndNodeWeight = 0;
    int gotoStartNodeWeight = 0;
    int sumWeight = 0;

    int rowIndex = 0;
    int columnIndex = 0;

    bool isVisit = false;

    TileNode* parent = nullptr;
};

const float MAP_MOVE_SPEED = 20.0f;
const float MAP_BORDER_MARGIN = 10.0f;
const float MAP_BOTTOM_MARGIN = 200.0F;
const float MAP_MIN_SCALE = 0.3f;
const float MAP_MAX_SCALE = 1.0f;
const int LINEUP_MAX_NPC_COUNT_PER_LINE = 5;

// TileNode的标记
const int OBSTACLE_ID = 2;
const int PASSABLE_ID = 0;

class MapManager
{
public:
    ~MapManager();

    bool init(Node* parentNode, const std::string& titleMapFileName);

    void updateMapPosition();
    void updateMapScale(Event* event);

    void syncCursorPoint(const Vec2& cursorPoint);

    void setTileMapPosition(const Vec2& worldPosition);

    Size getTileSize();
    Size getMapSize();
    Point getTileSubscript(const Vec2& inMapPosition);
    MapDebugInfo getMapDebugInfo(TileMapLayerType tileMapLayerType);
    float getMapScale();
    void setMapScale(float scaleValue);
    const Vec2& getMapPosition();
    const Vector<TMXObjectGroup*>&  getEditedGameObjectGroup();

    Vec2 convertCursorPositionToTileMapSpace();
    Vec2 convertToTileMapSpace(const Vec2& worldPosition);

    void addChildInGameObjectLayer(Node* gameObject, int zOrder = 1);

    TileNode* getTileNodeAt(int columnIndex, int rowIndex);

    bool isInObstacleTile(const Vec2& inMapPosition);

    void showMopUpCommandTips();
    void hideMopUpCommandTips();

    void showMoveCommandTips();
    void hideMoveCommandTips();
private:
    void resolveMapShakeWhenMove();

    void initTileNodeTable();
    void initMopUpCommandTips();
    void initMoveCommandTips();

    cocos2d::experimental::TMXTiledMap* _tileMap = nullptr;
    float _mapScale = 0.5f; // 由于设计上的问题，这个值不能改，一旦修改，编辑后的建筑物位置会错位

    RECT _clientRect;
    Vec2 _cursorPoint;

    vector<vector<TileNode*>> _tileNodeTable;

    Sprite* _mopUpCommandTips = nullptr;
    Sprite* _moveCommandTips = nullptr;
};