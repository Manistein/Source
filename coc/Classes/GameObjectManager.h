#pragma once

#include "GameObject.h"

class GameObject;

class GameObjectManager
{
public:
    static GameObjectManager* getInstance();
    GameObject* createGameObject(GameObjectType type, const string& jobName, const Vec2& position);
    void removeGameObjectBy(int uniqueID);
    void removeAllGameObjects();
private:
    unordered_map<int, GameObject*> _gameObjectMap;

    GameObjectManager(){}
    GameObjectManager(const GameObjectManager&);
    GameObjectManager& operator = (const GameObjectManager&);
};