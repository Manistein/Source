#include "Base.h"
#include "GameObject.h"
#include "GameObjectManager.h"
#include "GameWorld.h"
#include "Npc.h"

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

    _npcReadyMoveToEndPositionDataMap[ForceType::Player] = NPC_READY_MOVE_TO_END_POSITION_DATA();
    _npcReadyMoveToEndPositionDataMap[ForceType::AI] = NPC_READY_MOVE_TO_END_POSITION_DATA();
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
        gameObjectIter.second->depthSort(tileSize);
    }
}

bool GameObjectManager::selectGameObjectsBy(const Rect& rect)
{
    bool result = false;

    for (auto& gameObjectIter : _gameObjectMap)
    {
        if (gameObjectIter.second->isDying())
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
        if (gameObjectIter.second->isDying())
        {
            continue;
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
        if (gameObjectIter.second->isDying() || gameObjectIter.second->getForceType() != ForceType::AI)
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

void GameObjectManager::setSelectedNpcMoveEndPositionList(ForceType forceType, const vector<Vec2>& npcMoveEndPositionList)
{
    if (npcMoveEndPositionList.empty() || forceType == ForceType::Invalid)
    {
        return;
    }

    _npcReadyMoveToEndPositionDataMap[forceType]._readyMoveToEndPositionNpcIDList.clear();
    _npcReadyMoveToEndPositionDataMap[forceType]._npcMoveEndPositionList.clear();

    int index = 0;
    for (auto& gameObjectIter : _gameObjectMap)
    {
        if (gameObjectIter.second->getGameObjectType() != GameObjectType::Npc)
        {
            continue;
        }

        if (gameObjectIter.second->isSelected() && gameObjectIter.second->getForceType() == forceType)
        {
            if (index >= (int)npcMoveEndPositionList.size())
            {
                break;
            }
            
            auto npc = static_cast<Npc*>(gameObjectIter.second);
            npc->setReadyToMoveStatus(true);
            _npcReadyMoveToEndPositionDataMap[forceType]._readyMoveToEndPositionNpcIDList.push_back(npc->getUniqueID());

            index++;
        }
    }

    _npcReadyMoveToEndPositionDataMap[forceType]._npcMoveEndPositionList.assign(npcMoveEndPositionList.begin(), npcMoveEndPositionList.end());
}


void GameObjectManager::npcMoveToEndPositionOneByOne()
{
    for (auto& dataIter : _npcReadyMoveToEndPositionDataMap)
    {
        if (dataIter.second._npcMoveEndPositionList.empty() || 
            dataIter.second._readyMoveToEndPositionNpcIDList.empty())
        {
            continue;
        }

        auto readyMoveToEndPositionNpcID = dataIter.second._readyMoveToEndPositionNpcIDList.back();
        auto readyMoveToEndPositionNpcIter = _gameObjectMap.find(readyMoveToEndPositionNpcID);
        if (readyMoveToEndPositionNpcIter == _gameObjectMap.end())
        {
            dataIter.second._readyMoveToEndPositionNpcIDList.pop_back();
            continue;
        }
        else
        {
            auto readyMoveToEndPositionNpc = static_cast<Npc*>(readyMoveToEndPositionNpcIter->second);
            if (readyMoveToEndPositionNpc->isReadyToRemove())
            {
                dataIter.second._readyMoveToEndPositionNpcIDList.pop_back();
                continue;
            }

            auto moveEndPosition = dataIter.second._npcMoveEndPositionList.back();
            readyMoveToEndPositionNpc->moveTo(moveEndPosition);

            dataIter.second._readyMoveToEndPositionNpcIDList.pop_back();
            dataIter.second._npcMoveEndPositionList.pop_back();
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
    auto parentNode = gameObject->getParent();
    auto worldPosition = parentNode->convertToWorldSpace(gameObject->getPosition());
    auto contentSize = gameObject->getContentSize();

    return Rect(worldPosition.x - contentSize.width / 2.0f, worldPosition.y - contentSize.height / 2.0f, contentSize.width, contentSize.height);
}
