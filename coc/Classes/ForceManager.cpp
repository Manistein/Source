#include "Base.h"
#include "GameObject.h"
#include "ForceManager.h"
#include "GameWorldCallBackFunctionsManager.h"
#include "GameObjectManager.h"
#include "Building.h"

static ForceManager* s_forceManager = nullptr;

ForceManager* ForceManager::getInstance()
{
    if (!s_forceManager)
    {
        s_forceManager = new ForceManager();
        s_forceManager->init();
    }

    return s_forceManager;
}

bool ForceManager::init()
{
    _enemyLaunchAttackCoolDownTime = ENEMY_LAUNCH_ATTACK_TIME_INTERVAL;
    _enemyReinforceCoolDownTime = ENEMY_REINFORCE_TIME_INTERVAL;
    _playerReinforceCoolDownTime = PLAYER_REINFORCE_TIME_INTERVAL;

    _forceDataMap[ForceType::Player] = ForceData();
    _forceDataMap[ForceType::AI] = ForceData();

    _gameWorld = GameWorldCallBackFunctionsManager::getInstance();

    return true;
}

void ForceManager::onEnemyLaunchAttack()
{
    auto gameObjectManager = GameObjectManager::getInstance();

}

void ForceManager::onEnemyReinforcementArrive()
{
    static const vector<string> enemyNpcTemplateNameList = { "PinkArcher", "BlueBarbarian", "PurpleEnchanter" };
    auto listIndex = rand() % (int)enemyNpcTemplateNameList.size();
    _gameWorld->_createNpcAroundBaseCamp(ForceType::AI, enemyNpcTemplateNameList[listIndex], 16);
}

void ForceManager::onPlayerReinforcePointIncrease()
{
    auto forceDataIter = _forceDataMap.find(ForceType::Player);
    forceDataIter->second.reinforcePoint++;
}

void ForceManager::update(float delta)
{
    _enemyLaunchAttackCoolDownTime -= delta;
    if (_enemyLaunchAttackCoolDownTime <= 0.0f)
    {
        _enemyLaunchAttackCoolDownTime = ENEMY_LAUNCH_ATTACK_TIME_INTERVAL;
        onEnemyLaunchAttack();
    }

    _enemyReinforceCoolDownTime -= delta;
    if (_enemyReinforceCoolDownTime <= 0.0f)
    {
        _enemyReinforceCoolDownTime = ENEMY_REINFORCE_TIME_INTERVAL;
        onEnemyReinforcementArrive();
    }

    _playerReinforceCoolDownTime -= delta;
    if (_playerReinforceCoolDownTime <= 0.0f)
    {
        _playerReinforceCoolDownTime = PLAYER_REINFORCE_TIME_INTERVAL;
        onPlayerReinforcePointIncrease();
    }
}
