#pragma once

class GameObject;
class GameWorld;

typedef unordered_map<int, GameObject*> GameObjectMap;

class GameObjectManager
{
public:
    static GameObjectManager* getInstance();
    void init(GameWorld* gameWorld);

    GameObject* createGameObject(GameObjectType gameObjectType, ForceType forceType, const string& jobName, const Vec2& position);
    void removeGameObjectBy(int uniqueID);
    void removeAllGameObjects();

    void addDecimatedGameObject(int gameObjcetUniqueID);
    void removeAllDecimatedGameObjects();

    GameObject* getGameObjectBy(int uniqueID);
    const GameObjectMap& getGameObjectMap();

    void gameObjectsDepthSort(const Size& tileSize);

    bool selectGameObjectsBy(const Rect& rect);
    GameObject* selectGameObjectBy(const Point& cursorPoint);
    GameObject* selectEnemyBy(const Point& cursorPoint);
    void cancelAllGameObjectSelected();
    void cancelEnemySelected();

    int getGameObjectSelectedByPlayerCount();

    void npcSelectedByPlayerMoveTo(const Vec2& position);
    void npcSelectedByPlayerMoveTo(const vector<Vec2>& npcMoveEndPositionList);

    void setSelectedGameObjectEnemyUniqueID(int uniqueID);
    void clearGameObjectDebugDraw();
private:
    GameObjectMap _gameObjectMap;

    GameObjectManager(){}
    GameObjectManager(const GameObjectManager&);
    GameObjectManager& operator = (const GameObjectManager&);

    GameWorld* _gameWorld = nullptr;

    vector<int> _decimatedGameObjectIDList;
};