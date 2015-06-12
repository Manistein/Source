#pragma once

class GameWorld;
class MapManager;
struct DebugInfo;

class GameWorldCallBackFunctionsManager
{
public:
    static GameWorldCallBackFunctionsManager* getInstance();

    void registerCallBackFunctions(GameWorld* gameWorld);

    std::function<void(BulletType, int, int)> _createBullet;
    std::function<void(int)> _removeGameObjectBy;
    std::function<list<Vec2>(const Vec2&, const Vec2&, bool)> _computePathList;
    std::function<MapManager*()> _getMapManager;
    std::function<void(const string&, const Vec2&, bool)> _createSpecialEffect;
    std::function<const DebugInfo&()> _getDebugInfo;
    std::function<void(ForceType, const string&, int)> _createNpcAroundBaseCamp;
    std::function<int()> _getPlayerBaseCampUniqueID;
    std::function<int()> _getAIBaseCampUniqueID;
private:
    GameWorldCallBackFunctionsManager(){};
    GameWorldCallBackFunctionsManager(const GameWorldCallBackFunctionsManager&);
    GameWorldCallBackFunctionsManager& operator = (const GameWorldCallBackFunctionsManager&);
};