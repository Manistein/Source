#pragma once

class GameObject;

class GameObjectManager
{
public:
    static GameObjectManager* getInstance();

    GameObject* createGameObject(GameObjectType type, const string& jobName, const Vec2& position);
    void removeGameObjectBy(int uniqueID);
    void removeAllGameObjects();

    void gameObjectsDepthSort(const Size& tileSize);
    void selectGameObjectsBy(Rect rect);
    void selectedGameObjectsMoveTo(const Vec2& position);
private:
    unordered_map<int, GameObject*> _gameObjectMap;

    GameObjectManager(){}
    GameObjectManager(const GameObjectManager&);
    GameObjectManager& operator = (const GameObjectManager&);
};