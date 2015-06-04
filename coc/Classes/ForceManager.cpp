#include "Base.h"
#include "GameObject.h"
#include "ForceManager.h"
#include "GameWorldCallBackFunctionsManager.h"
#include "GameObjectManager.h"
#include "Building.h"
#include "Npc.h"

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
    _readyToMoveEnemyIDList.clear();
    vector<GameObject*> playerBuildingList;

    auto gameObjectManager = GameObjectManager::getInstance();
    auto& gameObjectMap = gameObjectManager->getGameObjectMap();
    for (auto& gameObjectIter : gameObjectMap)
    {
        auto gameObject = gameObjectIter.second;
        if (gameObject->getGameObjectType() == GameObjectType::Building &&
            gameObject->getForceType() == ForceType::Player)
        {
            playerBuildingList.push_back(gameObject);
        }
        else if (gameObject->getGameObjectType() == GameObjectType::Npc &&
            gameObject->getForceType() == ForceType::AI)
        {
            _readyToMoveEnemyIDList.push_back(gameObject->getUniqueID());
        }
    }

    if (playerBuildingList.empty())
    {
        return;
    }

    int buildingListIndex = rand() % (int)playerBuildingList.size();
    auto attackTarget = static_cast<Building*>(playerBuildingList.at(buildingListIndex));
    _enemyMoveToPosition = attackTarget->getBottomGridInMapPositionList().at(0);
    
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

    if (!_readyToMoveEnemyIDList.empty())
    {
        auto readyToMoveGameObjectID = _readyToMoveEnemyIDList.back();
        auto readyToMoveGameObject = GameObjectManager::getInstance()->getGameObjectBy(readyToMoveGameObjectID);
        if (readyToMoveGameObject &&
            !readyToMoveGameObject->isReadyToRemove() &&
            readyToMoveGameObject->getGameObjectType() == GameObjectType::Npc)
        {
            auto readyToMoveNpc = static_cast<Npc*>(readyToMoveGameObject);
            readyToMoveNpc->moveTo(_enemyMoveToPosition, true);

            _readyToMoveEnemyIDList.pop_back();
        }
    }
}
