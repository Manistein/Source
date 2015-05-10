#pragma once

class GameObject;
class GameWorld;
class Npc;

struct NPC_READY_MOVE_TO_END_POSITION_DATA
{
    list<int> _readyMoveToEndPositionNpcIDList;
    list<Vec2> _npcMoveEndPositionList;
};

typedef unordered_map<int, GameObject*> GameObjectMap;

class GameObjectManager
{
public:
    static GameObjectManager* getInstance();
    void init(GameWorld* gameWorld);

    GameObject* createGameObject(GameObjectType gameObjectType, ForceType forceType, const string& jobName, const Vec2& position);
    void removeGameObjectBy(int uniqueID);
    void removeAllGameObjects();

    void addReadyToRemoveGameObject(int gameObjcetUniqueID);
    void removeAllReadyToRemoveGameObjects();

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
    void setSelectedNpcMoveEndPositionList(ForceType forceType, const vector<Vec2>& npcMoveEndPositionList);
    void npcMoveToEndPositionOneByOne();

    void setSelectedGameObjectEnemyUniqueID(int uniqueID);
    void clearGameObjectDebugDraw();
private:
    Rect computeGameObjectRect(GameObject* gameObject);

    GameObjectMap _gameObjectMap;
    unordered_map<ForceType, NPC_READY_MOVE_TO_END_POSITION_DATA> _npcReadyMoveToEndPositionDataMap;

    GameObjectManager(){}
    GameObjectManager(const GameObjectManager&);
    GameObjectManager& operator = (const GameObjectManager&);

    GameWorld* _gameWorld = nullptr;

    vector<int> _readyToRemoveGameObjectIDList;
};