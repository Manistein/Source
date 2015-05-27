#include "Base.h"
#include "GameObject.h"
#include "Building.h"
#include "TemplatesManager.h"
#include "GameWorldCallBackFunctionsManager.h"
#include "MapManager.h"
#include <functional>
#include "GameObjectManager.h"
#include "Npc.h"

const int MAX_BOTTOM_GRID_COUNT = 9;
const int MIN_BOTTOM_GRID_COUNT = 1;
const float BUILDING_DELAY_REMOVE_TIME = 2.0f;

const string ENABLE_BUILD_GRID_FILE_NAME = "EnableBuildGBrid.png";
const string DISABLE_BUILD_GRID_FILE_NAME = "DisableBuildGrid.png";
const string BEING_BUILT_PROGRESS_BAR = "PlayerHPBar.png";

Building* Building::create(ForceType forceType, const string& buildingTemplateName, const Vec2& position, int uniqueID)
{
    auto building = new Building();
    if (building && building->init(forceType, buildingTemplateName, position, uniqueID))
    {
        building->autorelease();
    }
    else
    {
        CC_SAFE_DELETE(building);
    }

    return building;
}

bool Building::init(ForceType forceType, const string& buildingTemplateName, const Vec2& position, int uniqueID)
{
    _forceType = forceType;

    if (!GameObject::init())
    {
        return false;
    }

    _buildingTemplateName = buildingTemplateName;

    _gameObjectType = GameObjectType::Building;
    _buildingStatus = BuildingStatus::PrepareToBuild;
    setPosition(position);
    _uniqueID = uniqueID;

    initBuildingStatusSprites(buildingTemplateName);
    initBottomGridSprites(buildingTemplateName);

    // 所有势力的Building都需要先更新为PrepareToBuild状态，
    updateStatus(BuildingStatus::PrepareToBuild);

    if (_forceType == ForceType::AI)
    {
        // 只有当building被addChild到tileMap时，才能准确计算占用哪些格子，因此需要延时更新到BeingBuilt状态，
        delayUpdateAIForceBuildingToBeingBuiltStatus(position);
    }

    initHPBar();
    initBeingBuiltProgressBar();
    initBattleData(buildingTemplateName);

    scheduleUpdate();

    return true;
}

void Building::initBuildingStatusSprites(const string& buildingTemplateName)
{
    createBuildingStatusSprite(buildingTemplateName, BuildingStatus::PrepareToBuild, 180);
    createBuildingStatusSprite(buildingTemplateName, BuildingStatus::BeingBuilt);
    createBuildingStatusSprite(buildingTemplateName, BuildingStatus::Working);
    createBuildingStatusSprite(buildingTemplateName, BuildingStatus::Destory);
}

Sprite* Building::createBuildingStatusSprite(const string& buildingTemplateName, BuildingStatus buildingStatus, int opacity /* = 255 */)
{
    auto buildingTemplate = TemplateManager::getInstance()->getBuildingTemplateBy(buildingTemplateName);
    auto spriteFrameCache = SpriteFrameCache::getInstance();

    float shadowYPosition = 0.0f;
    string spriteTextureName;
    bool hasShadow = false;

    switch (buildingStatus)
    {
    case BuildingStatus::PrepareToBuild:
        spriteTextureName = buildingTemplate->prepareToBuildStatusTextureName;
        break;
    case BuildingStatus::BeingBuilt:
        shadowYPosition = buildingTemplate->shadowYPositionInBeingBuiltStatus;
        spriteTextureName = buildingTemplate->beingBuiltStatusTextureName;
        hasShadow = true;
        break;
    case BuildingStatus::Working:
        shadowYPosition = buildingTemplate->shadowYPositionInWorkingStatus;
        spriteTextureName = buildingTemplate->workingStatusTextureName;
        hasShadow = true;
        break;
    case BuildingStatus::Destory:
        shadowYPosition = buildingTemplate->shadowYPositionInDestroyStatus;
        spriteTextureName = buildingTemplate->destroyStatusTextureName;
        hasShadow = true;
        break;
    default:    break;
    }

    auto buildingStatusSpriteFrame = spriteFrameCache->getSpriteFrameByName(spriteTextureName);
    auto buildingStatusSprite = Sprite::create();
    buildingStatusSprite->setSpriteFrame(buildingStatusSpriteFrame);
    buildingStatusSprite->setOpacity(opacity);
    addChild(buildingStatusSprite);
    _buildingStatusSpriteMap[buildingStatus] = buildingStatusSprite;

    if (hasShadow)
    {
        auto buildStatusSpriteSize = buildingStatusSprite->getContentSize();

        auto shadowSprite = Sprite::create();
        auto shadowSpriteFrame = spriteFrameCache->getSpriteFrameByName(buildingTemplate->shadowTextureName);
        shadowSprite->setSpriteFrame(shadowSpriteFrame);
        shadowSprite->setPosition(Vec2(buildStatusSpriteSize.width / 2.0f, shadowYPosition));
        buildingStatusSprite->addChild(shadowSprite, -1);
    }

    return buildingStatusSprite;
}

void Building::initBottomGridSprites(const string& buildingTemplateName)
{
    auto buildingTemplate = TemplateManager::getInstance()->getBuildingTemplateBy(buildingTemplateName);

    MapManager* mapManager = GameWorldCallBackFunctionsManager::getInstance()->_getMapManager();
    auto tileSize = mapManager->getTileSize();

    auto prepareToBuildSprite = _buildingStatusSpriteMap[BuildingStatus::PrepareToBuild];
    auto prepareToBuildSpriteSize = prepareToBuildSprite->getContentSize();

    Vec2 centerBottomGridPosition(prepareToBuildSprite->getContentSize().width / 2.0f, buildingTemplate->centerBottomGridYPosition);
    Vec2 firstQueryGridPosition(centerBottomGridPosition.x, centerBottomGridPosition.y + tileSize.height);
    _bottomGridsPlaneCenterPositionInLocalSpace = centerBottomGridPosition;

    for (int i = 0; i < buildingTemplate->bottomGridRowCount; i++)
    {
        Vec2 tempGridPostion;

        tempGridPostion.x = firstQueryGridPosition.x - i * tileSize.width / 2.0f;
        tempGridPostion.y = firstQueryGridPosition.y - i * tileSize.height / 2.0f;

        for (int j = 0; j < buildingTemplate->bottomGridColumnCount; j++)
        {
            auto gridSprite = Sprite::create();
            auto gridSpriteFrame = SpriteFrameCache::getInstance()->getSpriteFrameByName(ENABLE_BUILD_GRID_FILE_NAME);
            gridSprite->setSpriteFrame(gridSpriteFrame);
            gridSprite->setPosition(tempGridPostion);
            prepareToBuildSprite->addChild(gridSprite, -1);

            _bottomGridSpritesList.push_back(gridSprite);

            tempGridPostion.x += tileSize.width / 2.0f;
            tempGridPostion.y -= tileSize.height / 2.0f;
        }
    }
}

void Building::initHPBar()
{
    auto contentSize = _buildingStatusSpriteMap[BuildingStatus::Working]->getContentSize();

    auto hpBarBackground = _hpBar->getParent();
    hpBarBackground->setPosition(Vec2(contentSize.width / 2.0f, contentSize.height + 50.0f));
}

void Building::initBeingBuiltProgressBar()
{
    _beingBuildProgressBar = ui::LoadingBar::create(BEING_BUILT_PROGRESS_BAR);
    _beingBuildProgressBar->setAnchorPoint(Vec2::ZERO);
    _beingBuildProgressBar->setPercent(0.0f);

    auto beingBuiltProgressBarBackground = Sprite::create(HP_BAR_BACKGROUND_TEXTURE_NAME);
    beingBuiltProgressBarBackground->setCascadeOpacityEnabled(true);
    beingBuiltProgressBarBackground->setScale(0.5f);
    beingBuiltProgressBarBackground->addChild(_beingBuildProgressBar);
    beingBuiltProgressBarBackground->setVisible(false);
    addChild(beingBuiltProgressBarBackground);

    auto contentSize = _buildingStatusSpriteMap[BuildingStatus::BeingBuilt]->getContentSize();
    beingBuiltProgressBarBackground->setPosition(Vec2(contentSize.width / 2.0f, contentSize.height + 50.0f));
}

void Building::initBattleData(const string& buildingTemplateName)
{
    auto buildingTemplate = TemplateManager::getInstance()->getBuildingTemplateBy(buildingTemplateName);

    _maxHp = _hp = buildingTemplate->maxHP;
    _buildingTimeBySecond = buildingTemplate->buildingTimeBySecond;
    _extraEnemyAttackRadius = buildingTemplate->extraEnemyAttackRadius;
    _destroySpecialEffectTemplateName = buildingTemplate->destroySpecialEffectTemplateName;
}

Npc* Building::createDefenceNpc(const string& buildingTemplateName)
{
    Npc* defenceNpc = nullptr;

    auto buildingTemplate = TemplateManager::getInstance()->getBuildingTemplateBy(buildingTemplateName);
    if (buildingTemplate->defenceNpcName != "" && buildingTemplate->defenceNpcName != "Null")
    {
        auto workingStatusBuildingSprite = _buildingStatusSpriteMap[BuildingStatus::Working];
        auto workingStatusBuildingSpriteSize = workingStatusBuildingSprite->getContentSize();

        auto gameObjectManager = GameObjectManager::getInstance();
        auto gameObject = gameObjectManager->createGameObject(GameObjectType::DefenceInBuildingNpc, 
            _forceType, 
            buildingTemplate->defenceNpcName, 
            Vec2(workingStatusBuildingSpriteSize.width / 2.0f, 
            buildingTemplate->defenceNpcYPosition));

        defenceNpc = static_cast<Npc*>(gameObject);
        workingStatusBuildingSprite->addChild(defenceNpc, 1);
        defenceNpc->initDefenceInBuildingNpcInMapPosition();
    }

    return defenceNpc;
}

void Building::removeDefenceNpc()
{
    if (_defenceNpc != nullptr)
    {
        auto npcGID = _defenceNpc->getUniqueID();
        GameObjectManager::getInstance()->removeGameObjectBy(npcGID);
        _defenceNpc = nullptr;
    }
}

void Building::clear()
{

}

bool Building::isReadyToRemove()
{
    return _buildingStatus == BuildingStatus::Destory;
}

void Building::update(float delta)
{
    GameObject::update(delta);

    followCursorInPrepareToBuildStatus();
    updateBottomGridTextureInPrepareToBuildStatus();

    if (_buildingStatus == BuildingStatus::BeingBuilt)
    {
        hideHPBar();
        updateBeingBuiltProgressBar(delta);
    }
}

void Building::updateStatus(BuildingStatus buildingStatus)
{
    for (auto& buildStatusSpriteIter : _buildingStatusSpriteMap)
    {
        if (buildStatusSpriteIter.first == buildingStatus)
        {
            buildStatusSpriteIter.second->setVisible(true);

            auto buildingSize = buildStatusSpriteIter.second->getContentSize();
            setContentSize(buildingSize);
            buildStatusSpriteIter.second->setPosition(Vec2(buildingSize.width / 2.0f, buildingSize.height / 2.0f));

            switch (buildingStatus)
            {
            case BuildingStatus::BeingBuilt:
            {
                updateCoveredByBuildingTileNodesGID(OBSTACLE_ID);

                hideHPBar();
                showBeingBuiltProgressBar();

                auto onUpdateToWorkingStatus = CallFunc::create(CC_CALLBACK_0(Building::onConstructionComplete, this));
                auto sequenceAction = Sequence::create(DelayTime::create(_buildingTimeBySecond), onUpdateToWorkingStatus, nullptr);
                runAction(sequenceAction);
            }
                break;
            case BuildingStatus::Working:
            {
                _defenceNpc = createDefenceNpc(_buildingTemplateName);

                // 因为有些建筑物不需要经历建造阶段就可以直接进入working状态，因此这里需要再次更新占用格子的gid
                updateCoveredByBuildingTileNodesGID(OBSTACLE_ID);
            }
                break;
            case BuildingStatus::Destory:
            {
                if (_defenceNpc)
                {
                    removeDefenceNpc();
                }

                updateCoveredByBuildingTileNodesGID(PASSABLE_ID);

                GameWorldCallBackFunctionsManager::getInstance()->_createSpecialEffect(_destroySpecialEffectTemplateName, getPosition(), false);

                auto onReadyToRemove = CallFunc::create(CC_CALLBACK_0(Building::addToRemoveQueue, this));
                auto sequenceAction = Sequence::create(DelayTime::create(BUILDING_DELAY_REMOVE_TIME), onReadyToRemove, nullptr);
                runAction(sequenceAction);
            }
                break;
            default:    break;
            }
        }
        else
        {
            buildStatusSpriteIter.second->setVisible(false);
        }
    }

    _buildingStatus = buildingStatus;
}

BuildingStatus Building::getBuildingStatus()
{
    return _buildingStatus;
}

bool Building::canBuild()
{
    return _canBuild;
}

void Building::onPrepareToRemove()
{
    stopAllActions();
    hideBeingBuiltProgressBar();

    updateStatus(BuildingStatus::Destory);
}

void Building::debugDraw()
{
    _debugDrawNode->clear();
    _debugDrawNode->setVisible(true);

    auto contentSize = getContentSize();
    Rect gameObjectRect(0.0f, 0.0f, contentSize.width, contentSize.height);
    _debugDrawNode->drawRect(Vec2(gameObjectRect.getMinX(), gameObjectRect.getMinY()),
        Vec2(gameObjectRect.getMaxX(), gameObjectRect.getMaxY()),
        Color4F(0.0f, 0.0f, 1.0f, 0.5f));
}

void Building::followCursorInPrepareToBuildStatus()
{
    if (_buildingStatus != BuildingStatus::PrepareToBuild || _forceType == ForceType::AI)
    {
        return;
    }

    auto mapManager = GameWorldCallBackFunctionsManager::getInstance()->_getMapManager();
    auto cursorPointInTileMap = mapManager->convertCursorPositionToTileMapSpace();

    ajustBuildingPosition(cursorPointInTileMap);
}

void Building::ajustBuildingPosition(const Vec2& inMapPosition)
{
    auto mapManager = GameWorldCallBackFunctionsManager::getInstance()->_getMapManager();
    auto prepareToBuildSprite = _buildingStatusSpriteMap[BuildingStatus::PrepareToBuild];
    auto prepareToBuildSpriteSize = prepareToBuildSprite->getContentSize();

    // 为了保证底座和地图内的格子保持对齐，并且保证底座中心位置位于鼠标光标上，需要按照如下步骤做
    // 1）计算当底座中心位于鼠标光标上时，buiding的坐标，这里需要先setPosition，因为后面要取到经过位移后的bottomGrid的新坐标
    // 2）找到bottomGrid的中心坐标，位于那一个tileNode内，并且获取tileNode的中心坐标
    // 3）计算bottomGrid到tileNode中心的差值，并且让building坐标加上这个差值并且再次setPosition，以达到对齐效果
    Vec2 buildingPosition;
    buildingPosition.x = inMapPosition.x;
    buildingPosition.y = prepareToBuildSpriteSize.height / 2.0f - _bottomGridsPlaneCenterPositionInLocalSpace.y +
        inMapPosition.y;

    setPosition(buildingPosition);

    auto bottomGrid = _bottomGridSpritesList.front();
    auto bottomGridWorldPosition = prepareToBuildSprite->convertToWorldSpace(bottomGrid->getPosition());
    auto bottomGridInMapPosition = mapManager->convertToTileMapSpace(bottomGridWorldPosition);

    auto tileSize = mapManager->getTileSize();
    auto bottomGridSubscript = mapManager->getTileSubscript(bottomGridInMapPosition);
    auto tileNode = mapManager->getTileNodeAt((int)bottomGridSubscript.x, (int)bottomGridSubscript.y);
    auto tileTopPosition = tileNode->leftTopPosition;

    Vec2 tileCenterPosition(tileTopPosition.x, tileTopPosition.y - tileSize.height / 2.0f);
    buildingPosition.x += tileCenterPosition.x - bottomGridInMapPosition.x;
    buildingPosition.y += tileCenterPosition.y - bottomGridInMapPosition.y;

    setPosition(buildingPosition);
}

bool Building::isBottomGridCoverNpc(const Vec2& bottomGridInMapPosition)
{
    bool result = false;

    auto mapManager = GameWorldCallBackFunctionsManager::getInstance()->_getMapManager();
    auto coveredByBottomGridTileNodeSubscript = mapManager->getTileSubscript(bottomGridInMapPosition);
    auto coveredByBotomGridTileNode = mapManager->getTileNodeAt((int)coveredByBottomGridTileNodeSubscript.x, (int)coveredByBottomGridTileNodeSubscript.y);

    auto gameObjectManager = GameObjectManager::getInstance();
    auto gameObjectMap = gameObjectManager->getGameObjectMap();
    for (auto& gameObjectIter : gameObjectMap)
    {
        if (gameObjectIter.second->getGameObjectType() != GameObjectType::Npc)
        {
            continue;
        }

        auto gameObjectPosition = gameObjectIter.second->getPosition();
        auto coveredByNpcTileNodeSubscript = mapManager->getTileSubscript(gameObjectPosition);
        auto coveredByNpcTileNode = mapManager->getTileNodeAt((int)coveredByNpcTileNodeSubscript.x, (int)coveredByNpcTileNodeSubscript.y);
        if (coveredByBotomGridTileNode == coveredByNpcTileNode)
        {
            result = true;
            break;
        }
    }

    return result;
}

void Building::updateBottomGridTextureInPrepareToBuildStatus()
{
    if (_buildingStatus != BuildingStatus::PrepareToBuild)
    {
        return;
    }

    _canBuild = true;

    auto mapManager = GameWorldCallBackFunctionsManager::getInstance()->_getMapManager();
    auto spriteFrameCache = SpriteFrameCache::getInstance();
    auto enableBuildSpriteFrame = spriteFrameCache->getSpriteFrameByName(ENABLE_BUILD_GRID_FILE_NAME);
    auto disableBuildSpriteFrame = spriteFrameCache->getSpriteFrameByName(DISABLE_BUILD_GRID_FILE_NAME);

    for (auto& bottomGridSprite : _bottomGridSpritesList)
    {
        auto prepareToBuildSprite = _buildingStatusSpriteMap[BuildingStatus::PrepareToBuild];
        auto bottomGridWorldPosition = prepareToBuildSprite->convertToWorldSpace(bottomGridSprite->getPosition());
        auto bottomGridTileMapPosition = mapManager->convertToTileMapSpace(bottomGridWorldPosition);

        bool isInObstacleTile = mapManager->isInObstacleTile(bottomGridTileMapPosition);
        bool isCurrentBottomGridCoveredNpc = isBottomGridCoverNpc(bottomGridTileMapPosition);
        if (isInObstacleTile || isCurrentBottomGridCoveredNpc)
        {
            bottomGridSprite->setSpriteFrame(disableBuildSpriteFrame);
            _canBuild = false;
        }
        else
        {
            bottomGridSprite->setSpriteFrame(enableBuildSpriteFrame);
        }
    }
}

void Building::updateCoveredByBuildingTileNodesGID(int tileNodeGID)
{
    auto bottomGridInMapPositionList = getBottomGridInMapPositionList();
    auto mapManager = GameWorldCallBackFunctionsManager::getInstance()->_getMapManager();

    for (auto& inMapPosition : bottomGridInMapPositionList)
    {
        auto coveredByBottomGridTileNodeSubscript = mapManager->getTileSubscript(inMapPosition);
        auto coveredByBottomGridTileNode = mapManager->getTileNodeAt((int)coveredByBottomGridTileNodeSubscript.x, (int)coveredByBottomGridTileNodeSubscript.y);

        coveredByBottomGridTileNode->gid = tileNodeGID;
    }
}

void Building::delayUpdateAIForceBuildingToBeingBuiltStatus(const Vec2& inMapPosition)
{
    auto onInitBuildingStatus = CallFunc::create(CC_CALLBACK_0(Building::onUpdateAIForceBuildingStatus, this, inMapPosition, BuildingStatus::BeingBuilt));
    auto sequenceAction = Sequence::create(DelayTime::create(0.017f), onInitBuildingStatus, nullptr);
    runAction(sequenceAction);
}

void Building::onUpdateAIForceBuildingStatus(const Vec2& inMapPosition, BuildingStatus buildingStatus)
{
    ajustBuildingPosition(inMapPosition);
    updateStatus(buildingStatus);
}

void Building::showBeingBuiltProgressBar()
{
    auto background = _beingBuildProgressBar->getParent();
    background->setVisible(true);
}

void Building::hideBeingBuiltProgressBar()
{
    auto background = _beingBuildProgressBar->getParent();
    background->setVisible(false);
}

void Building::onConstructionComplete()
{
    hideBeingBuiltProgressBar();
    showHPBar();

    updateStatus(BuildingStatus::Working);
}

void Building::updateBeingBuiltProgressBar(float delta)
{
    auto percent = _passTimeBySecondInBeingBuiltStatus / _buildingTimeBySecond * 100.0f;
    _beingBuildProgressBar->setPercent(percent);

    _passTimeBySecondInBeingBuiltStatus += delta;
}

void Building::addToRemoveQueue()
{
    GameObjectManager::getInstance()->addReadyToRemoveGameObject(_uniqueID);
}

vector<Vec2> Building::getBottomGridInMapPositionList()
{
    vector<Vec2> bottomGridInMapPositionList;

    auto mapManager = GameWorldCallBackFunctionsManager::getInstance()->_getMapManager();
    auto parentNode = _bottomGridSpritesList.at(0)->getParent();

    for (auto& bottomGrid : _bottomGridSpritesList)
    {
        auto bottomGridWorldPosition = parentNode->convertToWorldSpace(bottomGrid->getPosition());
        auto bottomGridInMapPosition = mapManager->convertToTileMapSpace(bottomGridWorldPosition);

        bottomGridInMapPositionList.push_back(bottomGridInMapPosition);
    }

    return bottomGridInMapPositionList;
}
