#include "Base.h"
#include "GameObject.h"
#include "GameWorldCallBackFunctionsManager.h"
#include "GameWorld.h"

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
    _computePathListBetween = CC_CALLBACK_2(GameWorld::computePathListBetween, gameWorld);
    _getMapManager = CC_CALLBACK_0(GameWorld::getMapManager, gameWorld);
    _createSpecialEffect = CC_CALLBACK_3(GameWorld::createSpecialEffect, gameWorld);
}
