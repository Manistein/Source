#pragma once
#include "GameObject.h"

class MapManager;
class Npc;
class GameObjectManager;
class GameObjectSelectBox;

class GameWorld : public Node
{
public:
    ~GameWorld();
	CREATE_FUNC(GameWorld);

    void createGameObject(GameObjectType type, const string& jobName, const Vec2& position);
    void removeGameObjectBy(int uniqueID);

    void onMouseButtonDown();
    void onMouseButtonUp();

    void syncCursorPoint(const Vec2& cursorPoint);

    MapManager* _mapManager = nullptr;
private:
    bool init() override;
    void update(float deltaTime) override;
    void onMouseScroll(Event* event);

    GameObjectManager* _gameObjectManager = nullptr;
    Npc* _testNpc = nullptr;

    GameObjectSelectBox* _gameObjectSelectBox = nullptr;
    Vec2 _cursorPoint;
};