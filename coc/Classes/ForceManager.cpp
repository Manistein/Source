#include "Base.h"
#include "GameObject.h"
#include "ForceManager.h"
#include "GameWorldCallBackFunctionsManager.h"
#include "GameObjectManager.h"
#include "Building.h"
#include "Npc.h"
#include "GameConfigManager.h"

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

    _forceDataMap[ForceType::Player] = ForceData();

    auto reinforceConfig = GameConfigManager::getInstance()->getReinforceConfigBy(ForceType::AI);
    _forceDataMap[ForceType::AI] = ForceData();
    auto& reinforcementCountMap = _forceDataMap[ForceType::AI].reinforcementCountMap;
    reinforcementCountMap[reinforceConfig->enchanterTemplateName] = reinforceConfig->enchanterReinforceCount;
    reinforcementCountMap[reinforceConfig->archerTemplateName] = reinforceConfig->archerReinforceCount;
    reinforcementCountMap[reinforceConfig->barbarianTemplateName] = reinforceConfig->barbarianReinforceCount;

    _gameWorld = GameWorldCallBackFunctionsManager::getInstance();

    return true;
}

void ForceManager::onEnemyLaunchAttack()
{
    _readyToMoveEnemyIDList.clear();
    _playerBuildingList.clear();

    auto gameObjectManager = GameObjectManager::getInstance();
    auto& gameObjectMap = gameObjectManager->getGameObjectMap();
    for (auto& gameObjectIter : gameObjectMap)
    {
        auto gameObject = gameObjectIter.second;
        if (gameObject->getGameObjectType() == GameObjectType::Building &&
            gameObject->getForceType() == ForceType::Player)
        {
            auto building = static_cast<Building*>(gameObject);
            if (building->getBuildingStatus() != BuildingStatus::PrepareToBuild)
            {
                _playerBuildingList.push_back(gameObject);
            }  
        }
        else if (gameObject->getGameObjectType() == GameObjectType::Npc &&
            gameObject->getForceType() == ForceType::AI)
        {
            _readyToMoveEnemyIDList.push_back(gameObject->getUniqueID());
        }
    }

    if (_playerBuildingList.empty())
    {
        return;
    }

    _enemyMoveToPosition = computeEnemyMoveToPosition();
}

void ForceManager::onEnemyReinforcementArrive()
{
    auto& reinforcementCountMap = _forceDataMap[ForceType::AI].reinforcementCountMap;
    vector<string> enemyNpcTemplateNameList;
    for (auto& iter : reinforcementCountMap)
    {
        enemyNpcTemplateNameList.push_back(iter.first);
    }

    auto listIndex = rand() % (int)enemyNpcTemplateNameList.size();
    auto& npcTemplateName = enemyNpcTemplateNameList[listIndex];
    auto npcCount = reinforcementCountMap[npcTemplateName];
    _gameWorld->_createNpcAroundBaseCamp(ForceType::AI, npcTemplateName, npcCount);
}

void ForceManager::onPlayerReinforcePointIncrease()
{
    auto forceDataIter = _forceDataMap.find(ForceType::Player);
    forceDataIter->second.reinforcePoint++;
}

void ForceManager::onPlayerReinforcePointReduce()
{
    auto forceDataIter = _forceDataMap.find(ForceType::Player);
    forceDataIter->second.reinforcePoint--;

    forceDataIter->second.reinforcePoint = std::max(forceDataIter->second.reinforcePoint, 0);
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

            MopUpCommand mopUpCommand;
            mopUpCommand.isExecuting = true;
            mopUpCommand.finalPosition = _enemyMoveToPosition;
            readyToMoveNpc->setMopUpCommand(mopUpCommand);

            if (readyToMoveNpc->getNpcStatus() == NpcStatus::Stand)
            {
                if (_playerBuildingList.empty())
                {
                    _readyToMoveEnemyIDList.clear();
                }
                else
                {
                    _enemyMoveToPosition = computeEnemyMoveToPosition();
                }
            }
            else
            {
                _readyToMoveEnemyIDList.pop_back();
            }
        }
    }
}

const ForceData& ForceManager::getForceDataBy(ForceType forceType)
{
    CCASSERT(forceType != ForceType::Invalid, "");
    return _forceDataMap[forceType];
}

cocos2d::Vec2 ForceManager::computeEnemyMoveToPosition()
{
    Vec2 moveToPosition;

    if (_playerBuildingList.empty())
    {
        return moveToPosition;
    }

    int buildingListIndex = rand() % (int)_playerBuildingList.size();
    auto attackTarget = static_cast<Building*>(_playerBuildingList.at(buildingListIndex));

    moveToPosition = attackTarget->getBottomGridInMapPositionList().at(0);
    _playerBuildingList.erase(_playerBuildingList.begin() + buildingListIndex);

    return moveToPosition;
}
