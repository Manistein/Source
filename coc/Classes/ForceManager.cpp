#include "Base.h"
#include "GameObject.h"
#include "ForceManager.h"
#include "GameWorldCallBackFunctionsManager.h"
#include "GameObjectManager.h"
#include "Building.h"
#include "Npc.h"
#include "GameConfigManager.h"
#include "StorageManager.h"
#include "GameSetting.h"

static ForceManager* s_forceManager = nullptr;

ForceManager* ForceManager::getInstance()
{
    if (!s_forceManager)
    {
        s_forceManager = new ForceManager();
    }

    return s_forceManager;
}

bool ForceManager::init()
{
    _enemyLaunchAttackCoolDownTime = ENEMY_LAUNCH_ATTACK_TIME_INTERVAL;
    _enemyReinforceCoolDownTime = ENEMY_REINFORCE_TIME_INTERVAL;

    _forceDataMap[ForceType::Player] = ForceData();
    _forceDataMap[ForceType::AI] = ForceData();

    _gameWorld = GameWorldCallBackFunctionsManager::getInstance();

    initGameObjectLevelMap();

    return true;
}

bool ForceManager::initGameObjectLevelMap()
{
    auto gameConfigManager = GameConfigManager::getInstance();
    auto playerReinforceConfig = gameConfigManager->getReinforceConfigBy(ForceType::Player);
    auto aiReinforceConfig = gameConfigManager->getReinforceConfigBy(ForceType::AI);

    _gameObjectLevelMap[playerReinforceConfig->archerTemplateName] = 0;
    _gameObjectLevelMap[playerReinforceConfig->enchanterTemplateName] = 0;
    _gameObjectLevelMap[playerReinforceConfig->barbarianTemplateName] = 0;
    _gameObjectLevelMap[playerReinforceConfig->archerTowerTemplateName] = 0;
    _gameObjectLevelMap[playerReinforceConfig->enchanterTowerTemplateName] = 0;
    _gameObjectLevelMap[playerReinforceConfig->balloonTemplateName] = 0;
    _gameObjectLevelMap[playerReinforceConfig->gargTemplateName] = 0;

    _gameObjectLevelMap[aiReinforceConfig->archerTemplateName] = 0;
    _gameObjectLevelMap[aiReinforceConfig->enchanterTemplateName] = 0;
    _gameObjectLevelMap[aiReinforceConfig->barbarianTemplateName] = 0;
    _gameObjectLevelMap[aiReinforceConfig->archerTowerTemplateName] = 0;
    _gameObjectLevelMap[aiReinforceConfig->enchanterTowerTemplateName] = 0;
    _gameObjectLevelMap[aiReinforceConfig->balloonTemplateName] = 0;
    _gameObjectLevelMap[aiReinforceConfig->gargTemplateName] = 0;

    _aiUpgradeTemplateNameList = {
        aiReinforceConfig->archerTemplateName,
        aiReinforceConfig->enchanterTemplateName,
        aiReinforceConfig->barbarianTemplateName,
        aiReinforceConfig->balloonTemplateName,
        aiReinforceConfig->gargTemplateName
    };

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
    int selectedStage = StorageManager::getInstance()->_stageData.playerSelectedStage;
    auto stageConfig = GameConfigManager::getInstance()->getStageConfigBy(selectedStage);
    CCASSERT(stageConfig, "");

    float difficultyLevelFactor = 0.0f;
    switch (g_setting.difficultyLevel)
    {
    case DifficultyLevel::Easy:
        difficultyLevelFactor = stageConfig->easyModeFactor;
        break;
    case DifficultyLevel::Normal:
        difficultyLevelFactor = stageConfig->normalModeFactor;
        break;
    case DifficultyLevel::Hard:
        difficultyLevelFactor = stageConfig->hardModeFactor;
        break;
    default:    break;
    }

    auto reinforceConfig = GameConfigManager::getInstance()->getReinforceConfigBy(ForceType::AI);

    struct ReinforceData
    {
        string templateName;
        int reinforceCount;

        ReinforceData(const string& argTemplateName, int argReinforceCount) :
            templateName(argTemplateName), reinforceCount(argReinforceCount)
        {

        }
    };

    vector<ReinforceData> enemyNpcReinforceDataList = {
        ReinforceData(reinforceConfig->archerTemplateName, reinforceConfig->archerReinforceCount * difficultyLevelFactor),
        ReinforceData(reinforceConfig->barbarianTemplateName, reinforceConfig->barbarianReinforceCount * difficultyLevelFactor),
        ReinforceData(reinforceConfig->enchanterTemplateName, reinforceConfig->enchanterReinforceCount * difficultyLevelFactor),
        ReinforceData(reinforceConfig->balloonTemplateName, reinforceConfig->balloonReinforceCount * difficultyLevelFactor),
        ReinforceData(reinforceConfig->gargTemplateName, reinforceConfig->gargReinforceCount * difficultyLevelFactor),
    };

    auto listIndex = rand() % (int)enemyNpcReinforceDataList.size();
    auto& reinforceData = enemyNpcReinforceDataList[listIndex];
    _gameWorld->_createReinfoecement(ForceType::AI, reinforceData.templateName, reinforceData.reinforceCount);
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

    if (canTryAIForceUpgrade(delta))
    {
        tryAIForceUpgrade();

        launchForbidTryAIForceUpgradeTimer();
    }
}

const ForceData& ForceManager::getForceDataBy(ForceType forceType)
{
    CCASSERT(forceType != ForceType::Invalid, "");
    return _forceDataMap[forceType];
}

int ForceManager::getGameObjectLevel(const string& templateName)
{
    int level = 0;

    auto iter = _gameObjectLevelMap.find(templateName);
    if (iter != _gameObjectLevelMap.end())
    {
        level = iter->second;
    }

    return level;
}

void ForceManager::setGameObjectLevel(const string& templateName, int level)
{
    auto iter = _gameObjectLevelMap.find(templateName);
    if (iter != _gameObjectLevelMap.end())
    {
        iter->second = level;
    }
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

void ForceManager::addTechnologyPoint(ForceType type, int technologyPoint)
{
    auto iter = _forceDataMap.find(type);
    if (iter != _forceDataMap.end())
    {
        auto& forceData = iter->second;
        forceData.technologyPoint += technologyPoint;
    }
}

void ForceManager::costTechnologyPoint(ForceType type, int technologyPoint)
{
    auto iter = _forceDataMap.find(type);
    if (iter != _forceDataMap.end())
    {
        auto& forceData = iter->second;
        forceData.technologyPoint -= technologyPoint;
        forceData.technologyPoint = std::max(forceData.technologyPoint, 0);
    }
}

void ForceManager::launchForbidTryAIForceUpgradeTimer()
{
    _tryAIForceUpgradeCoolDownTime = TRY_AI_FORCE_UPGRADE_COOL_DOWN_TIME;
}

bool ForceManager::canTryAIForceUpgrade(float delta)
{
    _tryAIForceUpgradeCoolDownTime -= delta;
    return _tryAIForceUpgradeCoolDownTime <= 0;
}

void ForceManager::tryAIForceUpgrade()
{
    auto gameConfigManager = GameConfigManager::getInstance();
    int& technologyPoint = _forceDataMap[ForceType::AI].technologyPoint;

    for (auto& templateName : _aiUpgradeTemplateNameList)
    {
        auto levelIter = _gameObjectLevelMap.find(templateName);
        if (levelIter == _gameObjectLevelMap.end())
        {
            continue;
        }

        int& level = levelIter->second;
        if (level >= MAX_LEVEL)
        {
            continue;
        }

        auto levelConfig = gameConfigManager->getGameObjectLevelConfig(templateName, level + 1);
        if (technologyPoint >= levelConfig->costTechnologyPoint)
        {
            technologyPoint -= levelConfig->costTechnologyPoint;

            level++;
        }
    }
}
