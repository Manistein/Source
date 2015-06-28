#include "Base.h"
#include "GameObject.h"
#include "GameObjectManager.h"
#include "MapManager.h"
#include "GameWorld.h"
#include "Npc.h"
#include "Building.h"
#include "GameWorldCallBackFunctionsManager.h"
#include "Utils.h"
#include "SoundManager.h"

static GameObjectManager* s_gameObjectManager = nullptr;

const float MAX_WIDTH_SPACE_BETWEEN_NPC_IN_LINEUP = 60.0f;
const float MAX_HEIGHT_SPACE_BETWEEN_NPC_IN_LINEUP = 60.0f;

class LessDistanceChecker
{
public:
    LessDistanceChecker(const Vec2& arrivePosition) :
        _arrivePosition(arrivePosition)
    {

    }

    bool operator() (int leftNpcID, int rightNpcId)
    {
        auto gameObjectManager = GameObjectManager::getInstance();
        auto& gameObjectMap = gameObjectManager->getGameObjectMap();

        auto leftNpcIter = gameObjectMap.find(leftNpcID);
        auto rightNpcIter = gameObjectMap.find(rightNpcId);

        if (leftNpcIter == gameObjectMap.end() || rightNpcIter == gameObjectMap.end())
        {
            return false;
        }

        float distanceFromLeftNpcToArrivePosition = GameUtils::computeDistanceBetween(leftNpcIter->second->getPosition(), _arrivePosition);
        float distanceFromRightNpcToArrivePosition = GameUtils::computeDistanceBetween(rightNpcIter->second->getPosition(), _arrivePosition);

        return distanceFromLeftNpcToArrivePosition < distanceFromRightNpcToArrivePosition;
    }

private:
    Vec2 _arrivePosition;
};

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

bool GameObjectManager::selectGameObjectsBy(const Rect& rect, const string templateName /*= ""*/)
{
    bool result = false;

    for (auto& gameObjectIter : _gameObjectMap)
    {
        if (gameObjectIter.second->isReadyToRemove() ||
            (templateName != "" && gameObjectIter.second->getTemplateName() != templateName))
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

            if (gameObjectIter.second->getGameObjectType() == GameObjectType::Npc &&
                gameObjectIter.second->getForceType() == ForceType::Player)
            {
                _belongPlayerSelectedNpcIDList.push_back(gameObjectIter.second->getUniqueID());
            }
        }
    }

    return result;
}

GameObject* GameObjectManager::getGameObjectContain(const Vec2& cursorPoint)
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

            break;
        }
    }

    return gameObject;
}

GameObject* GameObjectManager::selectGameObjectBy(const Point& cursorPoint)
{
    GameObject* gameObject = getGameObjectContain(cursorPoint);

    if (gameObject)
    {
        select(gameObject);
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

    _belongPlayerSelectedNpcIDList.clear();
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

void GameObjectManager::npcSelectedByPlayerMoveTo(const Vec2& position, bool hasRetrieveMopUpCommand, bool isAllowEndTileNodeToMoveIn)
{
    if ((int)_belongPlayerSelectedNpcIDList.size() == 1)
    {
        int gameObjectUniqueID = _belongPlayerSelectedNpcIDList.front();
        auto gameObjectIter = _gameObjectMap.find(gameObjectUniqueID);
        if (gameObjectIter != _gameObjectMap.end())
        {
            auto gameObject = gameObjectIter->second;
            if (gameObject->isReadyToRemove())
            {
                return;
            }

            auto npc = static_cast<Npc*>(gameObject);
            npc->moveTo(position, isAllowEndTileNodeToMoveIn);

            MopUpCommand mopUpCommand;
            mopUpCommand.isExecuting = hasRetrieveMopUpCommand;
            mopUpCommand.finalPosition = position;
            npc->setMopUpCommand(mopUpCommand);

            SoundManager::getInstance()->playNpcEffect(npc->getTemplateName(), NpcSoundEffectType::Move);
        }
    }
    else
    {
        _belongPlayerSelectedNpcIDList.sort(LessDistanceChecker(position));

        _npcReadyMoveToTargetDataMap[ForceType::Player]._npcMoveTargetList = computeBelongPlayerSelectedNpcArrivePositionList(position);

        _npcReadyMoveToTargetDataMap[ForceType::Player]._readyMoveToTargetNpcIDList = _belongPlayerSelectedNpcIDList;
        for (auto npcID : _belongPlayerSelectedNpcIDList)
        {
            auto gameObjectIter = _gameObjectMap.find(npcID);
            if (gameObjectIter != _gameObjectMap.end() && !gameObjectIter->second->isReadyToRemove())
            {
                auto npc = static_cast<Npc*>(gameObjectIter->second);
                npc->setReadyToMoveStatus(true);

                MopUpCommand mopUpCommand;
                mopUpCommand.isExecuting = hasRetrieveMopUpCommand;
                npc->setMopUpCommand(mopUpCommand);

                SoundManager::getInstance()->playNpcEffect(npc->getTemplateName(), NpcSoundEffectType::Move);
            }
        }
    }
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

        auto readyMoveToTargetNpcID = dataIter.second._readyMoveToTargetNpcIDList.front();
        auto readyMoveToTargetNpcIter = _gameObjectMap.find(readyMoveToTargetNpcID);
        if (readyMoveToTargetNpcIter == _gameObjectMap.end())
        {
            dataIter.second._readyMoveToTargetNpcIDList.pop_front();
            continue;
        }
        else
        {
            auto readyMoveToTargetNpc = static_cast<Npc*>(readyMoveToTargetNpcIter->second);
            if (readyMoveToTargetNpc->isReadyToRemove())
            {
                dataIter.second._readyMoveToTargetNpcIDList.pop_front();
                continue;
            }

            auto targetPosition = dataIter.second._npcMoveTargetList.front();
            readyMoveToTargetNpc->moveTo(targetPosition);
            if (readyMoveToTargetNpc->isExecutingMopUpCommand())
            {
                MopUpCommand mopUpCommand;
                mopUpCommand.isExecuting = true;
                mopUpCommand.finalPosition = targetPosition;
                readyMoveToTargetNpc->setMopUpCommand(mopUpCommand);
            }

            dataIter.second._readyMoveToTargetNpcIDList.pop_front();
            dataIter.second._npcMoveTargetList.pop_front();
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

void GameObjectManager::formSelectedPlayerNpcIntoTeamBy(int teamID)
{
    if (_belongPlayerSelectedNpcIDList.empty())
    {
        return;
    }

    for (auto& gameObjectID : _belongPlayerSelectedNpcIDList)
    {
        auto gameObjectIter = _gameObjectMap.find(gameObjectID);
        if (gameObjectIter != _gameObjectMap.end())
        {
            auto gameObject = gameObjectIter->second;
            if (gameObject->isReadyToRemove())
            {
                continue;
            }

            auto gameObjectTeamID = gameObject->getTeamID();
            auto& teamIter = _playerTeamMemberIDsMap.find(gameObjectTeamID);
            if (teamIter != _playerTeamMemberIDsMap.end() && gameObjectTeamID != teamID)
            {
                teamIter->second.remove(gameObjectID);
            }
        }
    }

    _playerTeamMemberIDsMap[teamID] = _belongPlayerSelectedNpcIDList;
}

void GameObjectManager::selectPlayerTeamMemberBy(int teamID)
{
    cancelAllGameObjectSelected();

    auto& teamIter = _playerTeamMemberIDsMap.find(teamID);
    if (teamIter != _playerTeamMemberIDsMap.end())
    {
        auto& teamMemberIDList = teamIter->second;
        for (auto& teamMemberID : teamMemberIDList)
        {
            auto gameObjectIter = _gameObjectMap.find(teamMemberID);
            if (gameObjectIter == _gameObjectMap.end() || gameObjectIter->second->isReadyToRemove())
            {
                continue;
            }

            auto gameObject = gameObjectIter->second;
            gameObject->setTeamID(teamID);
            gameObject->setSelected(true);
        }

        _belongPlayerSelectedNpcIDList = teamMemberIDList;
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

list<Vec2> GameObjectManager::computeBelongPlayerSelectedNpcArrivePositionList(const Vec2& arrivePosition)
{
    list<Vec2> arrivePositionList;
    if (_belongPlayerSelectedNpcIDList.empty())
    {
        return arrivePositionList;
    }

    GameObject* firstMoveNpc = nullptr;
    for (auto npcID : _belongPlayerSelectedNpcIDList)
    {
        auto npcIter = _gameObjectMap.find(npcID);
        if (npcIter == _gameObjectMap.end() || npcIter->second->isReadyToRemove())
        {
            continue;
        }

        firstMoveNpc = npcIter->second;
        break;
    }

    if (!firstMoveNpc)
    {
        return arrivePositionList;
    }


    Vec2 turnToNextLineDelta;       // 到达下一个兵站点要移动的距离
    Vec2 turnToNextLocationDelta;   // 从当前排第一个位置到达下一排第一个位置要移动的距离
    Vec2 startLocation;
    float moveVectorDegree = GameUtils::computeRotatedDegree(firstMoveNpc->getPosition(), arrivePosition);
    if ((moveVectorDegree >= 0.0f && moveVectorDegree < 22.5f) ||
        (moveVectorDegree >= 337.5 && moveVectorDegree < 360.0f)) // 向北移动
    {
        startLocation.x = arrivePosition.x - (LINEUP_MAX_NPC_COUNT_PER_LINE / 2) * MAX_WIDTH_SPACE_BETWEEN_NPC_IN_LINEUP;
        startLocation.y = arrivePosition.y;

        turnToNextLineDelta.x = 0.0f;
        turnToNextLineDelta.y = -MAX_HEIGHT_SPACE_BETWEEN_NPC_IN_LINEUP;

        turnToNextLocationDelta.x = MAX_WIDTH_SPACE_BETWEEN_NPC_IN_LINEUP;
        turnToNextLocationDelta.y = 0.0f;
    }
    else if (moveVectorDegree >= 22.5f && moveVectorDegree < 67.5f) // 东北
    {
        startLocation.x = arrivePosition.x - (LINEUP_MAX_NPC_COUNT_PER_LINE / 2) * MAX_WIDTH_SPACE_BETWEEN_NPC_IN_LINEUP;
        startLocation.y = arrivePosition.y + (LINEUP_MAX_NPC_COUNT_PER_LINE / 2) * MAX_HEIGHT_SPACE_BETWEEN_NPC_IN_LINEUP;

        turnToNextLineDelta.x = -MAX_WIDTH_SPACE_BETWEEN_NPC_IN_LINEUP;
        turnToNextLineDelta.y = -MAX_HEIGHT_SPACE_BETWEEN_NPC_IN_LINEUP;

        turnToNextLocationDelta.x = MAX_WIDTH_SPACE_BETWEEN_NPC_IN_LINEUP;
        turnToNextLocationDelta.y = -MAX_HEIGHT_SPACE_BETWEEN_NPC_IN_LINEUP;
    }
    else if (moveVectorDegree >= 67.5 && moveVectorDegree < 112.5f) // 东
    {
        startLocation.x = arrivePosition.x;
        startLocation.y = arrivePosition.y + (LINEUP_MAX_NPC_COUNT_PER_LINE / 2) * MAX_HEIGHT_SPACE_BETWEEN_NPC_IN_LINEUP;

        turnToNextLineDelta.x = -MAX_WIDTH_SPACE_BETWEEN_NPC_IN_LINEUP;
        turnToNextLineDelta.y = 0.0f;

        turnToNextLocationDelta.x = 0.0f;
        turnToNextLocationDelta.y = -MAX_HEIGHT_SPACE_BETWEEN_NPC_IN_LINEUP;
    }
    else if (moveVectorDegree >= 112.5f && moveVectorDegree < 157.5f) // 东南
    {
        startLocation.x = arrivePosition.x + (LINEUP_MAX_NPC_COUNT_PER_LINE / 2) * MAX_WIDTH_SPACE_BETWEEN_NPC_IN_LINEUP;
        startLocation.y = arrivePosition.y + (LINEUP_MAX_NPC_COUNT_PER_LINE / 2) * MAX_HEIGHT_SPACE_BETWEEN_NPC_IN_LINEUP;

        turnToNextLineDelta.x = -MAX_WIDTH_SPACE_BETWEEN_NPC_IN_LINEUP;
        turnToNextLineDelta.y = MAX_HEIGHT_SPACE_BETWEEN_NPC_IN_LINEUP;

        turnToNextLocationDelta.x = -MAX_WIDTH_SPACE_BETWEEN_NPC_IN_LINEUP;
        turnToNextLocationDelta.y = -MAX_HEIGHT_SPACE_BETWEEN_NPC_IN_LINEUP;
    }
    else if (moveVectorDegree >= 157.5f && moveVectorDegree < 202.5f) // 南
    {
        startLocation.x = arrivePosition.x - (LINEUP_MAX_NPC_COUNT_PER_LINE / 2) * MAX_WIDTH_SPACE_BETWEEN_NPC_IN_LINEUP;
        startLocation.y = arrivePosition.y;

        turnToNextLineDelta.x = 0.0f;
        turnToNextLineDelta.y = MAX_HEIGHT_SPACE_BETWEEN_NPC_IN_LINEUP;

        turnToNextLocationDelta.x = MAX_WIDTH_SPACE_BETWEEN_NPC_IN_LINEUP;
        turnToNextLocationDelta.y = 0.0f;
    }
    else if (moveVectorDegree >= 202.5f && moveVectorDegree < 247.5f) // 西南
    {
        startLocation.x = arrivePosition.x - (LINEUP_MAX_NPC_COUNT_PER_LINE / 2) * MAX_WIDTH_SPACE_BETWEEN_NPC_IN_LINEUP;
        startLocation.y = arrivePosition.y + (LINEUP_MAX_NPC_COUNT_PER_LINE / 2) * MAX_HEIGHT_SPACE_BETWEEN_NPC_IN_LINEUP;

        turnToNextLineDelta.x = MAX_WIDTH_SPACE_BETWEEN_NPC_IN_LINEUP;
        turnToNextLineDelta.y = MAX_HEIGHT_SPACE_BETWEEN_NPC_IN_LINEUP;

        turnToNextLocationDelta.x = MAX_WIDTH_SPACE_BETWEEN_NPC_IN_LINEUP;
        turnToNextLocationDelta.y = -MAX_HEIGHT_SPACE_BETWEEN_NPC_IN_LINEUP;
    }
    else if (moveVectorDegree >= 247.5f && moveVectorDegree < 292.5f) // 西
    {
        startLocation.x = arrivePosition.x;
        startLocation.y = arrivePosition.y + (LINEUP_MAX_NPC_COUNT_PER_LINE / 2) * MAX_HEIGHT_SPACE_BETWEEN_NPC_IN_LINEUP;

        turnToNextLineDelta.x = MAX_WIDTH_SPACE_BETWEEN_NPC_IN_LINEUP;
        turnToNextLineDelta.y = 0.0f;

        turnToNextLocationDelta.x = 0.0f;
        turnToNextLocationDelta.y = -MAX_HEIGHT_SPACE_BETWEEN_NPC_IN_LINEUP;
    }
    else // 西北
    {
        startLocation.x = arrivePosition.x + (LINEUP_MAX_NPC_COUNT_PER_LINE / 2) * MAX_WIDTH_SPACE_BETWEEN_NPC_IN_LINEUP;
        startLocation.y = arrivePosition.y + (LINEUP_MAX_NPC_COUNT_PER_LINE / 2) * MAX_HEIGHT_SPACE_BETWEEN_NPC_IN_LINEUP;

        turnToNextLineDelta.x = MAX_WIDTH_SPACE_BETWEEN_NPC_IN_LINEUP;
        turnToNextLineDelta.y = -MAX_HEIGHT_SPACE_BETWEEN_NPC_IN_LINEUP;

        turnToNextLocationDelta.x = -MAX_WIDTH_SPACE_BETWEEN_NPC_IN_LINEUP;
        turnToNextLocationDelta.y = -MAX_HEIGHT_SPACE_BETWEEN_NPC_IN_LINEUP;
    }


    auto mapManager = GameWorldCallBackFunctionsManager::getInstance()->_getMapManager();
    auto mapSize = mapManager->getMapSize();
    auto tileSize = mapManager->getTileSize();

    while (arrivePositionList.size() < _belongPlayerSelectedNpcIDList.size())
    {
        Vec2 currentLocation = startLocation;
        int arriveLocationCountInLine = 0;
        while (arriveLocationCountInLine < LINEUP_MAX_NPC_COUNT_PER_LINE)
        {
            if (!mapManager->isInObstacleTile(currentLocation))
            {
                arrivePositionList.push_back(currentLocation);
                arriveLocationCountInLine++;
            }

            currentLocation.x += turnToNextLocationDelta.x;
            currentLocation.y += turnToNextLocationDelta.y;

            if (currentLocation.x < 0.0f || 
                currentLocation.x >= mapSize.width * tileSize.width ||
                currentLocation.y < 0.0f ||
                currentLocation.y > mapSize.height * tileSize.height)
            {
                break;
            }
        }

        if (arriveLocationCountInLine == 0)
        {
            break;
        }

        startLocation.x += turnToNextLineDelta.x;
        startLocation.y += turnToNextLineDelta.y;
    }

    return arrivePositionList;
}

void GameObjectManager::select(GameObject* gameObject)
{
    CCASSERT(gameObject != nullptr, "");

    gameObject->setSelected(true);

    if (gameObject->getGameObjectType() == GameObjectType::Npc &&
        gameObject->getForceType() == ForceType::Player)
    {
        _belongPlayerSelectedNpcIDList.push_back(gameObject->getUniqueID());
    }
}

void GameObjectManager::cancelSelect(GameObject* gameObject)
{
    CCASSERT(gameObject != nullptr, "");

    gameObject->setSelected(false);
    gameObject->setTeamID(TEAM_INVALID_ID);

    if (gameObject->getGameObjectType() == GameObjectType::Npc &&
        gameObject->getForceType() == ForceType::Player)
    {
        _belongPlayerSelectedNpcIDList.remove(gameObject->getUniqueID());
    }
}

void GameObjectManager::gameObjectJumpIntoScreen(GameObject* gameObject)
{
    auto mapManager = GameWorldCallBackFunctionsManager::getInstance()->_getMapManager();

    auto parent = gameObject->getParent();
    auto gameObjectPosition = gameObject->getPosition();
    auto gameObjectWorldPosition = parent->convertToWorldSpace(gameObjectPosition);
    auto visibileSize = Director::getInstance()->getVisibleSize();
    Vec2 screenCenterPosition(visibileSize.width / 2.0f, visibileSize.height / 2.0f);

    auto moveVector = screenCenterPosition - gameObjectWorldPosition;

    auto mapPosition = mapManager->getMapPosition();
    mapManager->setTileMapPosition(mapPosition + moveVector);
}

void GameObjectManager::teamMemberJumpIntoScreenBy(int teamID)
{
    auto teamMemberListIter = _playerTeamMemberIDsMap.find(teamID);
    if (teamMemberListIter != _playerTeamMemberIDsMap.end())
    {
        auto& teamMemberIDList = teamMemberListIter->second;
        for (auto& teamMemberID : teamMemberIDList)
        {
            auto teamMemberIter = _gameObjectMap.find(teamMemberID);
            if (teamMemberIter != _gameObjectMap.end())
            {
                gameObjectJumpIntoScreen(teamMemberIter->second);
                break;
            }
        }
    }
}
