#include "Base.h"
#include "GameObject.h"
#include "GameObjectManager.h"
#include "GameWorld.h"
#include "Npc.h"
#include "Building.h"
#include "GameWorldCallBackFunctionsManager.h"
#include "MapManager.h"

static GameObjectManager* s_gameObjectManager = nullptr;

GameObjectManager* GameObjectManager::getInstance()
{
    if (!s_gameObjectManager)
    {
        s_gameObjectManager = new GameObjectManager();
    }

    return s_gameObjectManager;
}

void GameObjectManager::init(GameWorld* gameWorld)
{
    _gameWorld = gameWorld;

    _npcReadyMoveToTargetDataMap[ForceType::Player] = NPC_READY_MOVE_TO_END_POSITION_DATA();
    _npcReadyMoveToTargetDataMap[ForceType::AI] = NPC_READY_MOVE_TO_END_POSITION_DATA();
}

GameObject* GameObjectManager::createGameObject(GameObjectType gameObjectType, ForceType forceType, const string& jobName, const Vec2& position)
{
    GameObject* gameObject = GameObjectFactory::create(gameObjectType, forceType, jobName, position);
    if (gameObject)
    {
        _gameObjectMap[gameObject->getUniqueID()] = gameObject;
    }

    return gameObject;
}

void GameObjectManager::removeGameObjectBy(int uniqueID)
{
    auto gameObjectIter = _gameObjectMap.find(uniqueID);
    if (gameObjectIter != _gameObjectMap.end())
    {
        gameObjectIter->second->removeFromParent();
        _gameObjectMap.erase(uniqueID);
    }
}

void GameObjectManager::removeAllGameObjects()
{
    for (auto& gameObjectIter : _gameObjectMap)
    {
        gameObjectIter.second->removeFromParent();
    }
    _gameObjectMap.clear();
}

void GameObjectManager::addReadyToRemoveGameObject(int gameObjcetUniqueID)
{
    _readyToRemoveGameObjectIDList.push_back(gameObjcetUniqueID);
}

void GameObjectManager::removeAllReadyToRemoveGameObjects()
{
    for (auto gameObjectID : _readyToRemoveGameObjectIDList)
    {
        removeGameObjectBy(gameObjectID);
    }
    _readyToRemoveGameObjectIDList.clear();
}

GameObject* GameObjectManager::getGameObjectBy(int uniqueID)
{
    GameObject* gameObject = nullptr;

    auto gameObjectIter = _gameObjectMap.find(uniqueID);
    if (gameObjectIter != _gameObjectMap.end())
    {
        gameObject = gameObjectIter->second;
    }

    return gameObject;
}

const GameObjectMap& GameObjectManager::getGameObjectMap()
{
    return _gameObjectMap;
}

void GameObjectManager::gameObjectsDepthSort(const Size& tileSize)
{
    for (auto& gameObjectIter : _gameObjectMap)
    {
        if (gameObjectIter.second->getGameObjectType() == GameObjectType::Building)
        {
            auto building = static_cast<Building*>(gameObjectIter.second);
            if (building->getBuildingStatus() == BuildingStatus::PrepareToBuild)
            {
                gameObjectIter.second->setPositionZ(MAX_GAME_OBJECT_COUNT);
            }
            else
            {
                gameObjectIter.second->depthSort(tileSize);
            }
        }
        else if (gameObjectIter.second->getGameObjectType() != GameObjectType::DefenceInBuildingNpc)
        {
            gameObjectIter.second->depthSort(tileSize);
        }
    }
}

bool GameObjectManager::selectGameObjectsBy(const Rect& rect)
{
    bool result = false;

    for (auto& gameObjectIter : _gameObjectMap)
    {
        if (gameObjectIter.second->isReadyToRemove())
        {
            continue;
        }

        auto objectParent = gameObjectIter.second->getParent();

        auto objectPosition = gameObjectIter.second->getPosition();
        auto objectPositionInGameWorld = objectParent->convertToWorldSpace(objectPosition);

        if (rect.containsPoint(objectPositionInGameWorld))
        {
            result = true;
            gameObjectIter.second->setSelected(true);
        }
    }

    return result;
}

GameObject* GameObjectManager::selectGameObjectBy(const Point& cursorPoint)
{
    GameObject* gameObject = nullptr;

    for (auto& gameObjectIter : _gameObjectMap)
    {
        if (gameObjectIter.second->isReadyToRemove())
        {
            continue;
        }

        if (gameObjectIter.second->getGameObjectType() == GameObjectType::Building)
        {
            auto building = static_cast<Building*>(gameObjectIter.second);
            if (building->getBuildingStatus() == BuildingStatus::PrepareToBuild)
            {
                continue;
            }
        }

        auto gameObjectRect = computeGameObjectRect(gameObjectIter.second);

        if (gameObjectRect.containsPoint(cursorPoint))
        {
            gameObject = gameObjectIter.second;
            gameObjectIter.second->setSelected(true);

            break;
        }
    }

    return gameObject;
}

GameObject* GameObjectManager::selectEnemyBy(const Point& cursorPoint)
{
    GameObject* enemy = nullptr;

    for (auto& gameObjectIter : _gameObjectMap)
    {
        if (gameObjectIter.second->isReadyToRemove() || gameObjectIter.second->getForceType() != ForceType::AI)
        {
            continue;
        }

        auto gameObjectRect = computeGameObjectRect(gameObjectIter.second);

        if (gameObjectRect.containsPoint(cursorPoint))
        {
            enemy = gameObjectIter.second;
            gameObjectIter.second->setSelected(true);

            break;
        }
    }

    return enemy;
}

void GameObjectManager::cancelAllGameObjectSelected()
{
    for (auto& gameObjectIter : _gameObjectMap)
    {
        gameObjectIter.second->setSelected(false);
    }
}

void GameObjectManager::cancelEnemySelected()
{
    for (auto& gameObjectIter : _gameObjectMap)
    {
        if (gameObjectIter.second->getForceType() != ForceType::AI || !gameObjectIter.second->isSelected())
        {
            continue;
        }

        gameObjectIter.second->setSelected(false);
    }
}

int GameObjectManager::getGameObjectSelectedByPlayerCount()
{
    int count = 0;

    for (auto& gameObjectIter : _gameObjectMap)
    {
        if (gameObjectIter.second->isSelected() && gameObjectIter.second->getForceType() == ForceType::Player)
        {
            count++;
        }
    }

    return count;
}

void GameObjectManager::npcSelectedByPlayerMoveTo(const Vec2& position)
{
    for (auto& gameObjectIter : _gameObjectMap)
    {
        if (gameObjectIter.second->getGameObjectType() != GameObjectType::Npc)
        {
            continue;
        }

        if (gameObjectIter.second->isSelected() && gameObjectIter.second->getForceType() == ForceType::Player)
        {
            auto npc = static_cast<Npc*>(gameObjectIter.second);
            npc->moveTo(position);
        }
    }
}

void GameObjectManager::setSelectedNpcMoveTargetList(ForceType forceType, const vector<Vec2>& npcMoveTargetList)
{
    if (npcMoveTargetList.empty() || forceType == ForceType::Invalid)
    {
        return;
    }

    _npcReadyMoveToTargetDataMap[forceType]._readyMoveToTargetNpcIDList.clear();
    _npcReadyMoveToTargetDataMap[forceType]._npcMoveTargetList.clear();

    int index = 0;
    for (auto& gameObjectIter : _gameObjectMap)
    {
        if (gameObjectIter.second->getGameObjectType() != GameObjectType::Npc)
        {
            continue;
        }

        if (gameObjectIter.second->isSelected() && gameObjectIter.second->getForceType() == forceType)
        {
            if (index >= (int)npcMoveTargetList.size())
            {
                break;
            }
            
            auto npc = static_cast<Npc*>(gameObjectIter.second);
            npc->setReadyToMoveStatus(true);
            _npcReadyMoveToTargetDataMap[forceType]._readyMoveToTargetNpcIDList.push_back(npc->getUniqueID());

            index++;
        }
    }

    _npcReadyMoveToTargetDataMap[forceType]._npcMoveTargetList.assign(npcMoveTargetList.begin(), npcMoveTargetList.end());
}


void GameObjectManager::npcMoveToTargetOneByOne()
{
    for (auto& dataIter : _npcReadyMoveToTargetDataMap)
    {
        if (dataIter.second._npcMoveTargetList.empty() ||
            dataIter.second._readyMoveToTargetNpcIDList.empty())
        {
            continue;
        }

        auto readyMoveToTargetNpcID = dataIter.second._readyMoveToTargetNpcIDList.back();
        auto readyMoveToTargetNpcIter = _gameObjectMap.find(readyMoveToTargetNpcID);
        if (readyMoveToTargetNpcIter == _gameObjectMap.end())
        {
            dataIter.second._readyMoveToTargetNpcIDList.pop_back();
            continue;
        }
        else
        {
            auto readyMoveToTargetNpc = static_cast<Npc*>(readyMoveToTargetNpcIter->second);
            if (readyMoveToTargetNpc->isReadyToRemove())
            {
                dataIter.second._readyMoveToTargetNpcIDList.pop_back();
                continue;
            }

            auto targetPosition = dataIter.second._npcMoveTargetList.back();
            readyMoveToTargetNpc->moveTo(targetPosition);

            dataIter.second._readyMoveToTargetNpcIDList.pop_back();
            dataIter.second._npcMoveTargetList.pop_back();
        }
    }
}

void GameObjectManager::setSelectedGameObjectEnemyUniqueID(int uniqueID)
{
    for (auto& gameObject : _gameObjectMap)
    {
        if (!gameObject.second->isSelected() || gameObject.second->getForceType() != ForceType::Player)
        {
            continue;
        }

        gameObject.second->setEnemyUniqueID(uniqueID);
    }
}

void GameObjectManager::clearGameObjectDebugDraw()
{
    for (auto& gameObjectIter : _gameObjectMap)
    {
        gameObjectIter.second->clearDebugDraw();
    }
}

Rect GameObjectManager::computeGameObjectRect(GameObject* gameObject)
{
    auto mapManager = GameWorldCallBackFunctionsManager::getInstance()->_getMapManager();
    float mapScale = mapManager->getMapScale();

    auto parentNode = gameObject->getParent();
    auto worldPosition = parentNode->convertToWorldSpace(gameObject->getPosition());
    auto contentSize = gameObject->getContentSize() * mapScale;

    return Rect(worldPosition.x - contentSize.width / 2.0f, worldPosition.y - contentSize.height / 2.0f, contentSize.width, contentSize.height);
}
