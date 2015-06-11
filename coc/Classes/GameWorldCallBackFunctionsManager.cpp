#include "Base.h"
#include "GameObject.h"
#include "MapManager.h"
#include "GameWorld.h"
#include "GameWorldCallBackFunctionsManager.h"

static GameWorldCallBackFunctionsManager* s_gameWorldCallBackFunctionsManager = nullptr;

GameWorldCallBackFunctionsManager* GameWorldCallBackFunctionsManager::getInstance()
{
    if (!s_gameWorldCallBackFunctionsManager)
    {
        s_gameWorldCallBackFunctionsManager = new GameWorldCallBackFunctionsManager();
    }

    return s_gameWorldCallBackFunctionsManager;
}

void GameWorldCallBackFunctionsManager::registerCallBackFunctions(GameWorld* gameWorld)
{
    _createBullet = CC_CALLBACK_3(GameWorld::createBullet, gameWorld);
    _removeGameObjectBy = CC_CALLBACK_1(GameWorld::removeGameObjectBy, gameWorld);
    _computePathList = CC_CALLBACK_4(GameWorld::computePathList, gameWorld);
    _getMapManager = CC_CALLBACK_0(GameWorld::getMapManager, gameWorld);
    _createSpecialEffect = CC_CALLBACK_3(GameWorld::createSpecialEffect, gameWorld);
    _getDebugInfo = CC_CALLBACK_0(GameWorld::getDebugInfo, gameWorld);
    _createNpcAroundBaseCamp = CC_CALLBACK_3(GameWorld::createNpcAroundBaseCamp, gameWorld);
    _getAIBaseCampUniqueID = CC_CALLBACK_0(GameWorld::getEnemyBaseCampUniqueID, gameWorld);
    _getPlayerBaseCampUniqueID = CC_CALLBACK_0(GameWorld::getPlayerBaseCampUniqueID, gameWorld);
}
