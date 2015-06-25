#pragma once

struct ForceData
{
    int reinforcePoint = 0;
    int technologyPoint = 0; // 用于升级的科技点

    std::map<string, int> reinforcementCountMap;
};

const float ENEMY_LAUNCH_ATTACK_TIME_INTERVAL = 60;
const float ENEMY_REINFORCE_TIME_INTERVAL = 20;
const float PLAYER_REINFORCE_TIME_INTERVAL = 30;

class GameWorldCallBackFunctionsManager;

class ForceManager
{
public:
    static ForceManager* getInstance();
    void update(float delta);

    const ForceData& getForceDataBy(ForceType forceType);

    void onPlayerReinforcePointIncrease();
    void onPlayerReinforcePointReduce();
private:
    bool init();
    void onEnemyLaunchAttack();
    void onEnemyReinforcementArrive();
    Vec2 computeEnemyMoveToPosition();

    float _enemyLaunchAttackCoolDownTime = 0.0f;
    float _enemyReinforceCoolDownTime = 0.0f;

    list<int> _readyToMoveEnemyIDList;
    vector<GameObject*> _playerBuildingList;
    Vec2 _enemyMoveToPosition;

    map<ForceType, ForceData> _forceDataMap;
    GameWorldCallBackFunctionsManager* _gameWorld = nullptr;

    ForceManager(){}
    ForceManager(const ForceManager&);
    ForceManager& operator = (const ForceManager&);
};