#pragma once

class GameObject;
class GameWorld;
class Npc;

struct NPC_READY_MOVE_TO_END_POSITION_DATA
{
    list<int> _readyMoveToTargetNpcIDList;
    list<Vec2> _npcMoveTargetList;
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

    GameObject* getGameObjectContain(const Vec2& cursorPoint);
    bool selectGameObjectsBy(const Rect& rect, const string templateName = "");
    GameObject* selectGameObjectBy(const Point& cursorPoint);
    GameObject* selectEnemyBy(const Point& cursorPoint);
    void cancelAllGameObjectSelected();
    void cancelEnemySelected();

    void select(GameObject* gameObject);
    void cancelSelect(GameObject* gameObject);

    int getGameObjectSelectedByPlayerCount();

    void npcSelectedByPlayerMoveTo(const Vec2& position, bool isAllowEndTileNodeToMoveIn = false);
    void npcMoveToTargetOneByOne();

    void setSelectedGameObjectEnemyUniqueID(int uniqueID);
    void clearGameObjectDebugDraw();

    void formSelectedPlayerNpcIntoTeamBy(int teamID);
    void selectPlayerTeamMemberBy(int teamID);

    void gameObjectJumpIntoScreen(GameObject* gameObject);
    void teamMemberJumpIntoScreenBy(int teamID);
private:
    Rect computeGameObjectRect(GameObject* gameObject);
    list<Vec2> computeBelongPlayerSelectedNpcArrivePositionList(const Vec2& arrivePosition);

    GameObjectMap _gameObjectMap;
    unordered_map<ForceType, NPC_READY_MOVE_TO_END_POSITION_DATA> _npcReadyMoveToTargetDataMap;

    GameObjectManager(){}
    GameObjectManager(const GameObjectManager&);
    GameObjectManager& operator = (const GameObjectManager&);

    GameWorld* _gameWorld = nullptr;

    vector<int> _readyToRemoveGameObjectIDList;

    list<int> _belongPlayerSelectedNpcIDList;
    unordered_map<int, list<int>> _playerTeamMemberIDsMap;
};