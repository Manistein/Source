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

    GameObject* getGameObjectBy(int uniqueID);
    const GameObjectMap& getGameObjectMap();

    void gameObjectsDepthSort(const Size& tileSize);

    bool selectGameObjectsBy(const Rect& rect);
    bool selectGameObjectsBy(const Point& point);
    void cancelSelected();

    void selectedNpcMoveTo(const Vec2& position);
private:
    GameObjectMap _gameObjectMap;

    GameObjectManager(){}
    GameObjectManager(const GameObjectManager&);
    GameObjectManager& operator = (const GameObjectManager&);

    GameWorld* _gameWorld = nullptr;
};