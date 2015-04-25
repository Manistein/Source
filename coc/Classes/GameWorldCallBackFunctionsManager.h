#pragma once

class GameWorld;

class GameWorldCallBackFunctionsManager
{
public:
    static GameWorldCallBackFunctionsManager* getInstance();

    void registerCallBackFunctions(GameWorld* gameWorld);

    std::function<void(BulletType, int, int)> _createBullet;
    std::function<void(int)> _removeGameObjectBy;
    std::function<list<Vec2>(const Vec2&, const Vec2&)> _computePathListBetween;
private:
    GameWorldCallBackFunctionsManager(){};
    GameWorldCallBackFunctionsManager(const GameWorldCallBackFunctionsManager&);
    GameWorldCallBackFunctionsManager& operator = (const GameWorldCallBackFunctionsManager&);
};