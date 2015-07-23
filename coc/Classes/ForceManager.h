#pragma once

struct ForceData
{
    int reinforcePoint = 0;
    int technologyPoint = 0; // 用于升级的科技点
};

const float ENEMY_LAUNCH_ATTACK_TIME_INTERVAL = 60;
const float ENEMY_REINFORCE_TIME_INTERVAL = 20;
const float PLAYER_REINFORCE_TIME_INTERVAL = 30;

const float TRY_AI_FORCE_UPGRADE_COOL_DOWN_TIME = 180.0f;

class GameWorldCallBackFunctionsManager;

class ForceManager
{
public:
    static ForceManager* getInstance();
    void update(float delta);

    const ForceData& getForceDataBy(ForceType forceType);
    int getGameObjectLevel(const string& templateName);
    void setGameObjectLevel(const string& templateName, int level);

    void onPlayerReinforcePointIncrease();
    void onPlayerReinforcePointReduce();

    void addTechnologyPoint(ForceType type, int technologyPoint);
    void costTechnologyPoint(ForceType type, int technologyPoint);

    bool init();
private:
    bool initGameObjectLevelMap();
    
    void onEnemyLaunchAttack();
    void onEnemyReinforcementArrive();
    Vec2 computeEnemyMoveToPosition();

    void launchForbidTryAIForceUpgradeTimer();
    bool canTryAIForceUpgrade(float delta);
    void tryAIForceUpgrade();

    float _enemyLaunchAttackCoolDownTime = 0.0f;
    float _enemyReinforceCoolDownTime = 0.0f;

    list<int> _readyToMoveEnemyIDList;
    vector<GameObject*> _playerBuildingList;
    Vec2 _enemyMoveToPosition;

    map<ForceType, ForceData> _forceDataMap;
    GameWorldCallBackFunctionsManager* _gameWorld = nullptr;

    typedef string TemplateName;
    typedef int Level;
    map<TemplateName, Level> _gameObjectLevelMap;
    vector<string> _aiUpgradeTemplateNameList;
    float _tryAIForceUpgradeCoolDownTime = 0;

    ForceManager(){}
    ForceManager(const ForceManager&);
    ForceManager& operator = (const ForceManager&);
};