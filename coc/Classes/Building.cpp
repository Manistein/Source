#include "Base.h"
#include "GameObject.h"
#include "Building.h"
#include "TemplatesManager.h"
#include "GameWorldCallBackFunctionsManager.h"
#include "MapManager.h"
#include <functional>

const int MAX_BOTTOM_GRID_COUNT = 9;
const int MIN_BOTTOM_GRID_COUNT = 1;

const string ENABLE_BUILD_GRID_FILE_NAME = "EnableBuildGBrid.png";
const string DISABLE_BUILD_GRID_FILE_NAME = "DisableBuildGrid.png";

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

    _gameObjectType = GameObjectType::Building;
    _buildingStatus = BuildingStatus::PrepareToBuild;
    setPosition(position);
    _uniqueID = uniqueID;

    initBuildingStatusSprites(buildingTemplateName);
    initBottomGridSprites(buildingTemplateName);
    updateStatus(BuildingStatus::PrepareToBuild);
    initHPBar();
    initBattleData(buildingTemplateName);

    scheduleUpdate();

    return true;
}

void Building::initBuildingStatusSprites(const string& buildingTemplateName)
{
    auto buildingTemplate = TemplateManager::getInstance()->getBuildingTemplateBy(buildingTemplateName);
    auto spriteFrameCache = SpriteFrameCache::getInstance();

    auto prepareToBuildSpriteFrame = spriteFrameCache->getSpriteFrameByName(buildingTemplate->prepareToBuildStatusTextureName);
    auto prepareToBuildSprite = Sprite::create();
    prepareToBuildSprite->setSpriteFrame(prepareToBuildSpriteFrame);
    prepareToBuildSprite->setOpacity(180);
    addChild(prepareToBuildSprite);
    _buildingStatusSpriteMap[BuildingStatus::PrepareToBuild] = prepareToBuildSprite;

    auto beingBuiltSpriteFrame = spriteFrameCache->getSpriteFrameByName(buildingTemplate->beingBuiltStatusTextureName);
    auto beingBuiltSprite = Sprite::create();
    beingBuiltSprite->setSpriteFrame(beingBuiltSpriteFrame);
    addChild(beingBuiltSprite);
    _buildingStatusSpriteMap[BuildingStatus::BeingBuilt] = beingBuiltSprite;

    auto workingSpriteFrame = spriteFrameCache->getSpriteFrameByName(buildingTemplate->workingStatusTextureName);
    auto workingSprite = Sprite::create();
    workingSprite->setSpriteFrame(workingSpriteFrame);
    addChild(workingSprite);
    _buildingStatusSpriteMap[BuildingStatus::Working] = workingSprite;

    auto destroySpriteFrame = spriteFrameCache->getSpriteFrameByName(buildingTemplate->destroyStatusTextureName);
    auto destroySprite = Sprite::create();
    destroySprite->setSpriteFrame(destroySpriteFrame);
    addChild(destroySprite);
    _buildingStatusSpriteMap[BuildingStatus::Destory] = destroySprite;
}

void Building::initBottomGridSprites(const string& buildingTemplateName)
{
    auto buildingTemplate = TemplateManager::getInstance()->getBuildingTemplateBy(buildingTemplateName);

    MapManager* mapManager = GameWorldCallBackFunctionsManager::getInstance()->_getMapManager();
    auto tileSize = mapManager->getTileSize();

    auto prepareToBuildSprite = _buildingStatusSpriteMap[BuildingStatus::PrepareToBuild];
    auto prepareToBuildSpriteSize = prepareToBuildSprite->getContentSize();

    if (prepareToBuildSpriteSize.width <= tileSize.width && prepareToBuildSpriteSize.height <= tileSize.height)
    {
        Vec2 centerBottomGridPosition(prepareToBuildSprite->getContentSize().width / 2.0f, buildingTemplate->centerBottomGridYPosition);

        auto gridSprite = Sprite::create();
        auto gridSpriteFrame = SpriteFrameCache::getInstance()->getSpriteFrameByName(ENABLE_BUILD_GRID_FILE_NAME);
        gridSprite->setSpriteFrame(gridSpriteFrame);
        gridSprite->setPosition(centerBottomGridPosition);
        prepareToBuildSprite->addChild(gridSprite);

        _bottomGridSpritesList.push_back(gridSprite);
    }
    else
    {
        Vec2 centerBottomGridPosition(prepareToBuildSprite->getContentSize().width / 2.0f, buildingTemplate->centerBottomGridYPosition);
        Vec2 firstQueryGridPosition(centerBottomGridPosition.x, centerBottomGridPosition.y + tileSize.height);

        for (int i = 0; i < 3; i++)
        {
            Vec2 tempGridPostion;

            tempGridPostion.x = firstQueryGridPosition.x - i * tileSize.width / 2.0f;
            tempGridPostion.y = firstQueryGridPosition.y - i * tileSize.height / 2.0f;

            for (int j = 0; j < 3; j++)
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
}

void Building::initHPBar()
{
    auto contentSize = _buildingStatusSpriteMap[BuildingStatus::Working]->getContentSize();

    auto hpBarBackground = _hpBar->getParent();
    hpBarBackground->setPosition(Vec2(contentSize.width / 2.0f, contentSize.height + 50.0f));
}

void Building::initBattleData(const string& buildingTemplateName)
{
    auto buildingTemplate = TemplateManager::getInstance()->getBuildingTemplateBy(buildingTemplateName);

    _maxHp = _hp = buildingTemplate->maxHP;
}

void Building::clear()
{

}

bool Building::isReadyToRemove()
{
    return _buildingStatus == BuildingStatus::Destory;
}

void Building::clearDebugDraw()
{

}

void Building::update(float delta)
{
    GameObject::update(delta);

    followCursorInPrepareToBuildStatus();
    updateBottomGridTextureInPrepareToBuildStatus();
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

bool Building::canUpdateToWorkingStatus()
{
    return _canUpdateToWorkingStatus;
}

void Building::onPrepareToRemove()
{
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
    if (_buildingStatus != BuildingStatus::PrepareToBuild)
    {
        return;
    }

    auto mapManager = GameWorldCallBackFunctionsManager::getInstance()->_getMapManager();
    auto cursorPointInTileMap = mapManager->convertCursorPositionToTileMapSpace();

    setPosition(cursorPointInTileMap);
}

void Building::updateBottomGridTextureInPrepareToBuildStatus()
{
    if (_buildingStatus != BuildingStatus::PrepareToBuild)
    {
        return;
    }

    _canUpdateToWorkingStatus = true;

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
        if (isInObstacleTile)
        {
            bottomGridSprite->setSpriteFrame(disableBuildSpriteFrame);
            _canUpdateToWorkingStatus = false;
        }
        else
        {
            bottomGridSprite->setSpriteFrame(enableBuildSpriteFrame);
        }
    }
}
