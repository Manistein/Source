#pragma once

class MapManager;
class Npc;
class GameObjectManager;
class GameObjectSelectBox;
class BulletManager;

class GameWorld : public Node
{
public:
    ~GameWorld();
	CREATE_FUNC(GameWorld);

    void createGameObject(GameObjectType gameObjectType, ForceType forceType, const string& jobName, const Vec2& position);
    void removeGameObjectBy(int uniqueID);

    void createBullet(BulletType bulletType, int attackerID, int attackTargetID);

    void onMouseLeftButtonDown();
    void onMouseLeftButtonUp();
    void onMouseRightButtonDown();
    void onMouseRightButtonUp();
    void onMouseMove(EventCustom* eventCustom);
    void onClearDebugDraw();

    void syncCursorPoint(const Vec2& cursorPoint);

    list<Vec2> computePathListBetween(const Vec2& inMapStartPosition, const Vec2& inMapEndPosition);

    MapManager* _mapManager = nullptr;
private:
    bool init() override;
    void update(float deltaTime) override;
    void onMouseScroll(Event* event);

    GameObjectManager* _gameObjectManager = nullptr;
    GameObjectSelectBox* _gameObjectSelectBox = nullptr;
    BulletManager* _bulletManager = nullptr;

    Vec2 _cursorPoint;
    Vec2 _previousClickedCursorPoint;

    bool _isLeftMouseButtonDown = false;
};