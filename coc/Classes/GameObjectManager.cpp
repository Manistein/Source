#include "Base.h"
#include "GameObject.h"
#include "GameObjectManager.h"
#include "GameWorld.h"
#include "Npc.h"

static GameObjectManager* s_gameObjectManager = nullptr;
const float MOVE_TO_POINT_X_RANGE = 150.0f;
const float MOVE_TO_POINT_Y_RANGE = 150.0f;

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

void GameObjectManager::addDecimatedGameObject(int gameObjcetUniqueID)
{
    _decimatedGameObjectIDList.push_back(gameObjcetUniqueID);
}

void GameObjectManager::removeAllDecimatedGameObjects()
{
    for (auto gameObjectID : _decimatedGameObjectIDList)
    {
        removeGameObjectBy(gameObjectID);
    }
    _decimatedGameObjectIDList.clear();
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
        if (gameObjectIter.second->isDecimated())
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
        if (gameObjectIter.second->isDecimated())
        {
            continue;
        }

        auto parentNode = gameObjectIter.second->getParent();
        auto worldPosition = parentNode->convertToWorldSpace(gameObjectIter.second->getPosition());
        auto contentSize = gameObjectIter.second->getContentSize();

        Rect gameObjectRect(worldPosition.x - contentSize.width / 2.0f, worldPosition.y - contentSize.height / 2.0f, contentSize.width, contentSize.height);

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

    enemy = selectGameObjectBy(cursorPoint);
    if (enemy && enemy->getForceType() != ForceType::AI)
    {
        enemy->setSelected(false);
        enemy = nullptr;
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

void GameObjectManager::npcSelectedByPlayerMoveTo(const vector<Vec2>& npcMoveEndPositionList)
{
    if (npcMoveEndPositionList.empty())
    {
        return;
    }

    int index = 0;
    for (auto& gameObjectIter : _gameObjectMap)
    {
        if (gameObjectIter.second->getGameObjectType() != GameObjectType::Npc)
        {
            continue;
        }

        if (gameObjectIter.second->isSelected() && gameObjectIter.second->getForceType() == ForceType::Player)
        {
            if (index >= (int)npcMoveEndPositionList.size())
            {
                break;
            }

            auto position = npcMoveEndPositionList[index];
            index++;

            auto npc = static_cast<Npc*>(gameObjectIter.second);
            npc->moveTo(position);
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