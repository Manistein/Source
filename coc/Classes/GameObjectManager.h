#pragma once

class GameObject;
class GameWorld;

class GameObjectManager
{
public:
    static GameObjectManager* getInstance();
    void init(GameWorld* gameWorld);

    GameObject* createGameObject(GameObjectType gameObjectType, ForceType forceType, const string& jobName, const Vec2& position);
    void removeGameObjectBy(int uniqueID);
    void removeAllGameObjects();

    void gameObjectsDepthSort(const Size& tileSize);

    bool selectGameObjectsBy(const Rect& rect);
    bool selectGameObjectsBy(const Point& point);
    void cancelSelected();

    void selectedNpcMoveTo(const Vec2& position);
private:
    unordered_map<int, GameObject*> _gameObjectMap;

    GameObjectManager(){}
    GameObjectManager(const GameObjectManager&);
    GameObjectManager& operator = (const GameObjectManager&);

    GameWorld* _gameWorld = nullptr;
};