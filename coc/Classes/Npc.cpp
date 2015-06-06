#include "Base.h"
#include "GameObject.h"
#include "Npc.h"
#include "TemplatesManager.h"
#include <math.h>
#include "GameObjectManager.h"
#include "GameSetting.h"
#include "GameWorldCallBackFunctionsManager.h"
#include "Utils.h"
#include "Building.h"
#include <functional>
#include "math/CCGeometry.h"
#include "MapManager.h"
#include "CustomMoveBy.h"

const string SHADOW_TEXTURE_NAME = "Shadow.png";
const string SHOW_WORLD_POSITION_CHILD_NAME = "ShowWorldPositionChildName";
const string SHOW_NPC_STATUS_CHILD_NAME = "ShowNpcStatusChildName";

const int MOVE_TO_ACTION_TAG = 1;

Npc::~Npc()
{
    clear();
}

Npc* Npc::create(ForceType forceType, GameObjectType npcType, const string& jobName, const Vec2& position, int uniqueID)
{
    auto npc = new Npc();
    if (npc && npc->init(forceType, npcType, jobName, position, uniqueID))
    {
        npc->autorelease();
    }
    else
    {
        CC_SAFE_DELETE(npc);
    }

    return npc;
}

bool Npc::init(ForceType forceType, GameObjectType npcType, const string& jobName, const Vec2& position, int uniqueID)
{
    _forceType = forceType;

    if (!GameObject::init())
    {
        return false;
    }

    initAnimates(jobName);
    initSwitchStatusFunctions();
    initShadow(jobName);
    initHPBar(jobName);
    initBattleData(jobName);
    initDebugDraw();
    initSelectedTips(jobName);

    setPosition(position);
    _uniqueID = uniqueID;

    _gameObjectType = npcType;

    _gameWorld = GameWorldCallBackFunctionsManager::getInstance();

    scheduleUpdate();

    return true;
}

void Npc::clear()
{
    for (auto& moveAnimate : _moveAnimateMap)
    {
        CC_SAFE_RELEASE_NULL(moveAnimate.second);
    }
    _moveAnimateMap.clear();

    if (_dieAnimate)
    {
        CC_SAFE_RELEASE_NULL(_dieAnimate);
    }

    for (auto& standAnimate : _standAnimateMap)
    {
        CC_SAFE_RELEASE_NULL(standAnimate.second);
    }
    _standAnimateMap.clear();
}

void Npc::initAnimates(const string& jobName)
{
    auto npcTempalte = TemplateManager::getInstance()->getNpcTemplateBy(jobName);
    CCASSERT(npcTempalte, StringUtils::format("%s is not a npc type", jobName.c_str()).c_str());

    _moveAnimateMap[FaceDirection::FaceToEast] = createAnimateWidthPList(npcTempalte->moveToEastAnimationPList, npcTempalte->moveAnimateDelayPerUnit, NpcStatus::Move);
    _moveAnimateMap[FaceDirection::FaceToNorthEast] = createAnimateWidthPList(npcTempalte->moveToNorthEastAnimationPList, npcTempalte->moveAnimateDelayPerUnit, NpcStatus::Move);
    _moveAnimateMap[FaceDirection::FaceToNorthWest] = createAnimateWidthPList(npcTempalte->moveToNorthWestAnimationPList, npcTempalte->moveAnimateDelayPerUnit, NpcStatus::Move);
    _moveAnimateMap[FaceDirection::FaceToSouthEast] = createAnimateWidthPList(npcTempalte->moveToSouthEastAnimationPList, npcTempalte->moveAnimateDelayPerUnit, NpcStatus::Move);
    _moveAnimateMap[FaceDirection::FaceToSouthWest] = createAnimateWidthPList(npcTempalte->moveToSouthWestAnimationPList, npcTempalte->moveAnimateDelayPerUnit, NpcStatus::Move);
    _moveAnimateMap[FaceDirection::FaceToWest] = createAnimateWidthPList(npcTempalte->moveToWestAnimationPList, npcTempalte->moveAnimateDelayPerUnit, NpcStatus::Move);

    _dieAnimate = createAnimateWidthPList(npcTempalte->dieAnimationPList, npcTempalte->dieAnimateDelayPerUnit, NpcStatus::Die);

    _standAnimateMap[FaceDirection::FaceToEast] = createAnimateWidthPList(npcTempalte->standAndFaceToEastAnimationPList, npcTempalte->standAnimateDelayPerUnit, NpcStatus::Stand);
    _standAnimateMap[FaceDirection::FaceToNorthEast] = createAnimateWidthPList(npcTempalte->standAndFaceToNorthEastAnimationPList, npcTempalte->standAnimateDelayPerUnit, NpcStatus::Stand);
    _standAnimateMap[FaceDirection::FaceToNorthWest] = createAnimateWidthPList(npcTempalte->standAndFaceToNorthWestAnimationPList, npcTempalte->standAnimateDelayPerUnit, NpcStatus::Stand);
    _standAnimateMap[FaceDirection::FaceToSouthEast] = createAnimateWidthPList(npcTempalte->standAndFaceToSouthEastAnimationPList, npcTempalte->standAnimateDelayPerUnit, NpcStatus::Stand);
    _standAnimateMap[FaceDirection::FaceToSouthWest] = createAnimateWidthPList(npcTempalte->standAndFaceToSouthWestAnimationPList, npcTempalte->standAnimateDelayPerUnit, NpcStatus::Stand);
    _standAnimateMap[FaceDirection::FaceToWest] = createAnimateWidthPList(npcTempalte->standAndFaceToWestAnimationPList, npcTempalte->standAnimateDelayPerUnit, NpcStatus::Stand);

    float attackAnimateDelayPerUnit = 1.0f / (float)npcTempalte->perSecondAttackCount;
    _attackAnimateMap[FaceDirection::FaceToEast] = createAnimateWidthPList(npcTempalte->attackToEastAnimationPList, attackAnimateDelayPerUnit, NpcStatus::Attack);
    _attackAnimateMap[FaceDirection::FaceToNorthEast] = createAnimateWidthPList(npcTempalte->attackToNorthEastAnimationPList, attackAnimateDelayPerUnit, NpcStatus::Attack);
    _attackAnimateMap[FaceDirection::FaceToNorthWest] = createAnimateWidthPList(npcTempalte->attackToNorthWestAnimationPList, attackAnimateDelayPerUnit, NpcStatus::Attack);
    _attackAnimateMap[FaceDirection::FaceToSouthEast] = createAnimateWidthPList(npcTempalte->attackToSouthEastAnimationPList, attackAnimateDelayPerUnit, NpcStatus::Attack);
    _attackAnimateMap[FaceDirection::FaceToSouthWest] = createAnimateWidthPList(npcTempalte->attackToSouthWestAnimationPList, attackAnimateDelayPerUnit, NpcStatus::Attack);
    _attackAnimateMap[FaceDirection::FaceToWest] = createAnimateWidthPList(npcTempalte->attackToWestAnimationPList, attackAnimateDelayPerUnit, NpcStatus::Attack);

    _faceDirection = FaceDirection::FaceToSouthEast;
    runAction(_standAnimateMap[_faceDirection]);
}

void Npc::initSwitchStatusFunctions()
{
    registerSwitchStatusFunction(_switchStatusFunctions[(int)NpcStatus::Move][(int)NpcStatus::Move],
        std::bind(&Npc::canSwitchMoveToMove, this),
        std::bind(&Npc::switchMoveToMove, this));

    registerSwitchStatusFunction(_switchStatusFunctions[(int)NpcStatus::Move][(int)NpcStatus::Stand],
        std::bind(&Npc::canSwitchMoveToStand, this),
        std::bind(&Npc::switchMoveToStand, this));

    registerSwitchStatusFunction(_switchStatusFunctions[(int)NpcStatus::Move][(int)NpcStatus::Attack],
        std::bind(&Npc::canSwitchMoveToAttack, this),
        std::bind(&Npc::switchMoveToAttack, this));

    registerSwitchStatusFunction(_switchStatusFunctions[(int)NpcStatus::Move][(int)NpcStatus::Die],
        std::bind(&Npc::canSwitchMoveToDie, this),
        std::bind(&Npc::switchMoveToDie, this));

    registerSwitchStatusFunction(_switchStatusFunctions[(int)NpcStatus::Stand][(int)NpcStatus::Stand],
        std::bind(&Npc::canSwitchStandToStand, this),
        std::bind(&Npc::switchStandToStand, this));

    registerSwitchStatusFunction(_switchStatusFunctions[(int)NpcStatus::Stand][(int)NpcStatus::Move],
        std::bind(&Npc::canSwitchStandToMove, this),
        std::bind(&Npc::switchStandToMove, this));

    registerSwitchStatusFunction(_switchStatusFunctions[(int)NpcStatus::Stand][(int)NpcStatus::Attack],
        std::bind(&Npc::canSwitchStandToAttack, this),
        std::bind(&Npc::switchStandToAttack, this));

    registerSwitchStatusFunction(_switchStatusFunctions[(int)NpcStatus::Stand][(int)NpcStatus::Die],
        std::bind(&Npc::canSwitchStandToDie, this),
        std::bind(&Npc::switchStandToDie, this));

    registerSwitchStatusFunction(_switchStatusFunctions[(int)NpcStatus::Attack][(int)NpcStatus::Attack],
        std::bind(&Npc::canSwitchAttackToAttack, this),
        std::bind(&Npc::switchAttackToAttack, this));

    registerSwitchStatusFunction(_switchStatusFunctions[(int)NpcStatus::Attack][(int)NpcStatus::Move],
        std::bind(&Npc::canSwitchAttackToMove, this),
        std::bind(&Npc::switchAttackToMove, this));

    registerSwitchStatusFunction(_switchStatusFunctions[(int)NpcStatus::Attack][(int)NpcStatus::Stand],
        std::bind(&Npc::canSwitchAttackToStand, this),
        std::bind(&Npc::switchAttackToStand, this));

    registerSwitchStatusFunction(_switchStatusFunctions[(int)NpcStatus::Attack][(int)NpcStatus::Die],
        std::bind(&Npc::canSwitchAttackToDie, this),
        std::bind(&Npc::switchAttackToDie, this));

    registerSwitchStatusFunction(_switchStatusFunctions[(int)NpcStatus::Die][(int)NpcStatus::Die],
        std::bind(&Npc::canSwitchDieToDie, this),
        std::bind(&Npc::switchDieToDie, this));

    registerSwitchStatusFunction(_switchStatusFunctions[(int)NpcStatus::Die][(int)NpcStatus::Move],
        std::bind(&Npc::canSwitchDieToMove, this),
        std::bind(&Npc::switchDieToMove, this));

    registerSwitchStatusFunction(_switchStatusFunctions[(int)NpcStatus::Die][(int)NpcStatus::Attack],
        std::bind(&Npc::canSwitchDieToAttack, this),
        std::bind(&Npc::switchDieToAttack, this));

    registerSwitchStatusFunction(_switchStatusFunctions[(int)NpcStatus::Die][(int)NpcStatus::Stand],
        std::bind(&Npc::canSwitchDieToStand, this),
        std::bind(&Npc::switchDieToStand, this));
}

void Npc::initShadow(const string& jobName)
{
    auto npcTemplate = TemplateManager::getInstance()->getNpcTemplateBy(jobName);

    auto contentSize = getContentSize();
    auto position = getPosition();

    _shadowSprite = Sprite::create(SHADOW_TEXTURE_NAME);
    _shadowSprite->setPosition(Vec2(contentSize.width / 2.0f, npcTemplate->shadowYPosition));
    _shadowSprite->setScale(2.0f);
    addChild(_shadowSprite, -1);
}

void Npc::initHPBar(const string& jobName)
{
    auto npcTemplate = TemplateManager::getInstance()->getNpcTemplateBy(jobName);

    auto contentSize = getContentSize();
    auto position = getPosition();

    auto hpBarBackground = _hpBar->getParent();
    hpBarBackground->setPosition(Vec2(contentSize.width / 2.0f, npcTemplate->hpBarYPosition));
}

void Npc::initBattleData(const string& jobName)
{
    auto npcTemplate = TemplateManager::getInstance()->getNpcTemplateBy(jobName);
    _maxHp = npcTemplate->maxHp;
    _hp = _maxHp;
    _attackPower = npcTemplate->attackPower;
    _maxAttackRadius = npcTemplate->maxAttackRadius;
    _maxAlertRadius = npcTemplate->maxAlertRadius;
    _perSecondMoveSpeedByPixel = npcTemplate->perSecondMoveSpeedByPixel;
    _bulletType = npcTemplate->bulletType;
    _damageType = npcTemplate->damageType;
    _aoeDamageRadius = npcTemplate->aoeDamageRadius;
    _reinforceRadius = npcTemplate->reinforceRadius;
}

void Npc::initDebugDraw()
{
    auto contentSize = getContentSize();
    auto position = getPosition();

    _debugDrawNode->setPosition(Vec2(contentSize.width / 2.0f, contentSize.height / 4.0f));

    TTFConfig ttfConfig("arial.ttf", 30.0f);
    auto worldPositionLabel = Label::createWithTTF(ttfConfig, "");
    worldPositionLabel->setPosition(Vec2::ZERO);
    worldPositionLabel->setName(SHOW_WORLD_POSITION_CHILD_NAME);
    _debugDrawNode->addChild(worldPositionLabel, 1);

    auto npcStatusLabel = Label::createWithTTF(ttfConfig, "");
    npcStatusLabel->setPosition(Vec2(0.0f, 20.0f));
    npcStatusLabel->setName(SHOW_NPC_STATUS_CHILD_NAME);
    _debugDrawNode->addChild(npcStatusLabel, 1);
}

void Npc::initSelectedTips(const string& jobName)
{
    auto npcTemplate = TemplateManager::getInstance()->getNpcTemplateBy(jobName);
    auto spriteFrameCache = SpriteFrameCache::getInstance();
    auto blueSelectedTips = spriteFrameCache->getSpriteFrameByName(npcTemplate->blueSelectedTipsTextureName);
    auto redSelectedTips = spriteFrameCache->getSpriteFrameByName(npcTemplate->redSelectedTipsTextureName);

    if (_forceType == ForceType::Player)
    {
        _selectedTips->setSpriteFrame(blueSelectedTips);
    }
    else
    {
        _selectedTips->setSpriteFrame(redSelectedTips);
    }

    auto shadowPosition = _shadowSprite->getPosition();
    _selectedTips->setPosition(shadowPosition);
    _selectedTips->setVisible(false);

    _collisionRadius = _selectedTips->getContentSize().width / 2.0f;
}

void Npc::debugDraw()
{
    _debugDrawNode->clear();

    _debugDrawNode->setVisible(true);
    _debugDrawNode->drawCircle(Vec2::ZERO, _maxAttackRadius, CC_DEGREES_TO_RADIANS(360), 50, true, 1.0f, 1.0f, Color4F(1.0f, 0.0f, 0.0f, 0.5f));
    _debugDrawNode->drawCircle(Vec2::ZERO, _maxAlertRadius, CC_DEGREES_TO_RADIANS(360), 50, true, 1.0f, 1.0f, Color4F(1.0f, 1.0f, 1.0f, 0.5f));
    _debugDrawNode->drawCircle(Vec2::ZERO, _reinforceRadius, CC_DEGREES_TO_RADIANS(360), 50, true, 1.0f, 1.0f, Color4F(1.0f, 1.0f, 0.0f, 0.5f));
    _debugDrawNode->drawCircle(Vec2(0.0f, _contentSize.width / 4.0f), _collisionRadius, CC_DEGREES_TO_RADIANS(360), 50, false, 1.0f, 1.0f, Color4F(1.0f, 0.0f, 0.0f, 1.0f));

    auto contentSize = getContentSize();
    Rect gameObjectRect(-contentSize.width / 2.0f, 0.0f, contentSize.width, contentSize.height);
    _debugDrawNode->drawRect(Vec2(gameObjectRect.getMinX(), gameObjectRect.getMinY()),
        Vec2(gameObjectRect.getMaxX(), gameObjectRect.getMaxY()),
        Color4F(0.0f, 0.0f, 1.0f, 0.5f));

    auto parentNode = getParent();
    auto position = getPosition();
    auto worldPosition = parentNode->convertToWorldSpace(position);

    auto worldPositionLabel = _debugDrawNode->getChildByName<Label*>(SHOW_WORLD_POSITION_CHILD_NAME);
    worldPositionLabel->setString(StringUtils::format("cx = %.2f, cy = %.2f", worldPosition.x, worldPosition.y));

    auto npcStatusLabel = _debugDrawNode->getChildByName<Label*>(SHOW_NPC_STATUS_CHILD_NAME);
    string statusName = "Current Status: ";
    switch (_oldStatus)
    {
    case NpcStatus::Attack:
        statusName += "Attack";
        break;
    case NpcStatus::Move:
        statusName += "Move";
        break;
    case NpcStatus::Stand:
        statusName += "Stand";
        break;
    case NpcStatus::Die:
        statusName += "Die";
        break;
    default: break;
    }
    npcStatusLabel->setString(statusName);
}

void Npc::update(float delta)
{
    GameObject::update(delta);

    _searchEnemyCoolDownTime -= delta;

    if (_gameObjectType == GameObjectType::Npc)
    {
        collisionTest();
        runFightWithEnemyAI(delta);
    }
    else if (_gameObjectType == GameObjectType::DefenceInBuildingNpc)
    {
        hideHPBar();
        runDefenceInBuildingAI(delta);
    }
}

void Npc::tryUpdateStatus(NpcStatus newStatus)
{
    auto& canSwitch = _switchStatusFunctions[(int)_oldStatus][(int)newStatus].canSwitch;
    auto& switchFunction = _switchStatusFunctions[(int)_oldStatus][(int)newStatus].switchFunction;

    if (canSwitch())
    {
        switchFunction();
        _oldStatus = newStatus;
    }
}

void Npc::collisionTest()
{
    if (_oldStatus == NpcStatus::Die)
    {
        return;
    }

    int collisionCount = 0;
    float sumDifference = 0.0f;
    Vec2 sumVector;

    Vec2 selfFaceDirectionVector;
    switch (_faceDirection)
    {
    case FaceDirection::FaceToEast:
    {
        selfFaceDirectionVector.x = 1.0f;
        selfFaceDirectionVector.y = 0.0f;
    }
        break;
    case FaceDirection::FaceToNorthEast:
    {
        selfFaceDirectionVector.x = 1.0f;
        selfFaceDirectionVector.y = 1.0f;
    }
        break;
    case FaceDirection::FaceToNorthWest:
    {
        selfFaceDirectionVector.x = -1.0f;
        selfFaceDirectionVector.y = 1.0f;
    }
        break;
    case  FaceDirection::FaceToSouthEast:
    {
        selfFaceDirectionVector.x = 1.0f;
        selfFaceDirectionVector.y = -1.0f;
    }
        break;
    case FaceDirection::FaceToSouthWest:
    {
        selfFaceDirectionVector.x = -1.0f;
        selfFaceDirectionVector.y = -1.0f;
    }
        break;
    case FaceDirection::FaceToWest:
    {
        selfFaceDirectionVector.x = -1.0f;
        selfFaceDirectionVector.y = 0.0f;
    }
        break;
    default:    break;
    }

    bool hasCollision = false;

    auto& gameObjectMap = GameObjectManager::getInstance()->getGameObjectMap();
    for (auto& gameObjectIter : gameObjectMap)
    {
        auto gameObject = gameObjectIter.second;
        if (gameObject->isReadyToRemove() || 
            gameObject->getGameObjectType() == GameObjectType::DefenceInBuildingNpc ||
            gameObject->getUniqueID() == _uniqueID ||
            gameObject->getForceType() != _forceType)
        {
            continue;
        }

        auto unitMoveVector = _position - gameObject->getPosition();
        float dot = selfFaceDirectionVector.dot(unitMoveVector);

        float gameObjectCollisionRadius = gameObject->getCollisionRadius();

        float realDistance = GameUtils::computeDistanceBetween(_position, gameObject->getPosition());
        float constraintDistance = _collisionRadius + gameObjectCollisionRadius;
        if (realDistance < constraintDistance && dot < 0.0f)
        {
            hasCollision = true;

            unitMoveVector.normalize();

            sumVector += unitMoveVector;
            sumDifference += constraintDistance - realDistance;

            collisionCount++;
        }
    }

    if (hasCollision)
    {
        auto averageVector = sumVector / collisionCount;
        auto averageDifference = sumDifference / collisionCount;
        auto moveVector = averageVector * averageDifference;

        auto newPosition = _position + moveVector;
        auto mapManager = GameWorldCallBackFunctionsManager::getInstance()->_getMapManager();

        auto readyToMoveInTileNodeSubscript = mapManager->getTileSubscript(newPosition);
        auto readyToMoveInTileNode = mapManager->getTileNodeAt((int)readyToMoveInTileNodeSubscript.x, (int)readyToMoveInTileNodeSubscript.y);

        if (readyToMoveInTileNode->gid != OBSTACLE_ID)
        {
            setPosition(_position + moveVector);
        }
    }
}

void Npc::runFightWithEnemyAI(float delta)
{
    if (_oldStatus == NpcStatus::Die)
    {
        return;
    }

    if (_enemyUniqueID != ENEMY_UNIQUE_ID_INVALID)
    {
        auto enemy = GameObjectManager::getInstance()->getGameObjectBy(_enemyUniqueID);
        if (!enemy || enemy->isReadyToRemove())
        {
            setEnemyUniqueID(ENEMY_UNIQUE_ID_INVALID);

            if (_oldStatus == NpcStatus::Attack)
            {
                tryUpdateStatus(NpcStatus::Stand);
            }

            return;
        }

        if (isEnemyInAttackRange(enemy))
        {
            tryUpdateStatus(NpcStatus::Attack);
        }
        else if (isEnemyInAlertRange(enemy))
        {
            switch (_oldStatus)
            {
            case NpcStatus::Move:
            {
                _coolDownTimeInMoveStatus += delta;
                if (_coolDownTimeInMoveStatus >= IN_MOVE_STATUS_COOL_DOWN_TIME_INTERVAL)
                {
                    auto npcPosition = getPosition();
                    auto enemyPosition = computeArrivePositionBy(enemy);

                    if (_gotoTargetPositionPathList.empty() ||
                        !_gotoTargetPositionPathList.empty() && _gotoTargetPositionPathList.back() != enemyPosition)
                    {
                        _gotoTargetPositionPathList.clear();

                        if (enemy->getGameObjectType() == GameObjectType::Npc)
                        {
                            _gotoTargetPositionPathList = _gameWorld->_computePathListBetween(npcPosition, enemyPosition, false);
                        }
                        else
                        {
                            _gotoTargetPositionPathList = _gameWorld->_computePathListBetween(npcPosition, enemyPosition, true);
                        }

                        if (_gotoTargetPositionPathList.empty())
                        {
                            tryUpdateStatus(NpcStatus::Stand);
                        }
                        else
                        {
                            tryUpdateStatus(NpcStatus::Move);
                        }
                    }

                    _coolDownTimeInMoveStatus = 0.0f;
                }
            }
                break;
            case NpcStatus::Stand:
            case NpcStatus::Attack:
            {
                auto npcPosition = getPosition();
                auto enemyPosition = computeArrivePositionBy(enemy);

                _gotoTargetPositionPathList.clear();
                if (enemy->getGameObjectType() == GameObjectType::Npc)
                {
                    _gotoTargetPositionPathList = _gameWorld->_computePathListBetween(npcPosition, enemyPosition, false);
                }
                else
                {
                    _gotoTargetPositionPathList = _gameWorld->_computePathListBetween(npcPosition, enemyPosition, true);
                }

                if (_gotoTargetPositionPathList.empty())
                {
                    tryUpdateStatus(NpcStatus::Stand);
                }
                else
                {
                    tryUpdateStatus(NpcStatus::Move);
                }
            }
                break;
            case NpcStatus::Die:
            default:    break;
            }
        }
        else
        {
            //ai控制的npc脱战，则立即站立，玩家控制的npc脱战以后，仍然可以移动。
            if (_forceType == ForceType::Player)
            {
                return;
            }

            switch (_oldStatus)
            {
            case NpcStatus::Move:
            case NpcStatus::Attack:
            {
                tryUpdateStatus(NpcStatus::Stand);
            }
                break;
            case NpcStatus::Stand:
            case NpcStatus::Die:
            default:    break;
            }

            setEnemyUniqueID(ENEMY_UNIQUE_ID_INVALID);
        }
    }
    else
    {
        if (canSearchEnemy())
        {
            searchNearbyEnemy();
            bool canFindEnemy = _enemyUniqueID != GAME_OBJECT_UNIQUE_ID_INVALID;

            if (_forceType == ForceType::AI &&
                !canFindEnemy &&
                _oldStatus == NpcStatus::Stand)
            {
                auto needReinforceGameObject = searchNearestNeedReinforceGameObject();
                if (needReinforceGameObject)
                {
                    Vec2 arrivePosition;

                    if (needReinforceGameObject->getGameObjectType() == GameObjectType::Building)
                    {
                        auto needReinforceBuilding = static_cast<Building*>(needReinforceGameObject);
                        auto& bottomGridPositionList = needReinforceBuilding->getBottomGridInMapPositionList();
                        arrivePosition = bottomGridPositionList.at(0);
                    }
                    else
                    {
                        arrivePosition = needReinforceGameObject->getPosition();
                    }

                    moveTo(arrivePosition, true);
                }
            }
        }
    }
}

void Npc::runDefenceInBuildingAI(float delta)
{
    if (_enemyUniqueID != ENEMY_UNIQUE_ID_INVALID)
    {
        auto enemy = GameObjectManager::getInstance()->getGameObjectBy(_enemyUniqueID);
        if (!enemy || enemy->isReadyToRemove())
        {
            setEnemyUniqueID(ENEMY_UNIQUE_ID_INVALID);
            tryUpdateStatus(NpcStatus::Stand);

            return;
        }

        if (isEnemyInAttackRange(enemy))
        {
            tryUpdateStatus(NpcStatus::Attack);
        }
        else
        {
            setEnemyUniqueID(ENEMY_UNIQUE_ID_INVALID);
            tryUpdateStatus(NpcStatus::Stand);
        }
    }
    else
    {
        if (canSearchEnemy())
        {
            searchNearbyEnemy();
        }
    }
}

bool Npc::canSearchEnemy()
{
    bool result = false;

    if (_forceType == ForceType::Player && _oldStatus == NpcStatus::Move ||
        _isReadyToMove ||
        _searchEnemyCoolDownTime > 0.0f)
    {
        result = false;
    }
    else
    {
        _searchEnemyCoolDownTime = SEARCH_ENEMY_COOL_DOWN_TIME_INTERVAL;
        result = true;
    }

    return result;
}

void Npc::searchNearbyEnemy()
{
    auto& gameObjectMap = GameObjectManager::getInstance()->getGameObjectMap();
    for (auto& gameObjectIter : gameObjectMap)
    {
        Building* buildingObject = nullptr;
        if (gameObjectIter.second->getGameObjectType() == GameObjectType::Building)
        {
            buildingObject = static_cast<Building*>(gameObjectIter.second);
        }

        if (_forceType == gameObjectIter.second->getForceType() ||
            gameObjectIter.second->isReadyToRemove() ||
            (buildingObject && buildingObject->getBuildingStatus() == BuildingStatus::PrepareToBuild))
        {
            continue;
        }

        if (isEnemyInAttackRange(gameObjectIter.second) || isEnemyInAlertRange(gameObjectIter.second))
        {
            setEnemyUniqueID(gameObjectIter.first);

            break;
        }
    }
}

GameObject* Npc::searchNearestNeedReinforceGameObject()
{
    GameObject* needReinforceGameObject = nullptr;

    if (_forceType == ForceType::Player || 
        _gameObjectType == GameObjectType::DefenceInBuildingNpc)
    {
        return needReinforceGameObject;
    }

    int needReinforceGameObjectID = GAME_OBJECT_UNIQUE_ID_INVALID;

    float minDistance = FLT_MAX;
    auto& gameObjectMap = GameObjectManager::getInstance()->getGameObjectMap();
    for (auto& gameObjectIter : gameObjectMap)
    {
        auto gameObject = gameObjectIter.second;
        if (gameObject->getForceType() == ForceType::AI)
        {
            int enemyID = gameObject->getEnemyUniqueID();
            if (enemyID != GAME_OBJECT_UNIQUE_ID_INVALID)
            {
                float distance = GameUtils::computeDistanceBetween(getPosition(), gameObject->getPosition());
                if (distance < _reinforceRadius && distance < minDistance)
                {
                    minDistance = distance;
                    needReinforceGameObjectID = gameObject->getUniqueID();
                }
            }
        }
    }

    needReinforceGameObject = GameObjectManager::getInstance()->getGameObjectBy(needReinforceGameObjectID);
    return needReinforceGameObject;
}

bool Npc::isEnemyInAttackRange(GameObject* enemy)
{
    bool result = false;

    float distance = getDistanceFrom(enemy);
    float extraAttackRadius = enemy->getExtraEnemyAttackRadius();

    if (distance <= _maxAttackRadius + extraAttackRadius)
    {
        result = true;
    }

    return result;
}

bool Npc::isEnemyInAlertRange(GameObject* enemy)
{
    bool result = false;

    float distance = getDistanceFrom(enemy);

    if (distance <= _maxAlertRadius)
    {
        result = true;
    }

    return result;
}

bool Npc::isReadyToRemove()
{
    return _oldStatus == NpcStatus::Die;
}

cocos2d::Vec2 Npc::computeArrivePositionBy(GameObject* enemy)
{
    Vec2 arrivePosition;
    if (enemy->getGameObjectType() == GameObjectType::Npc)
    {
        arrivePosition = enemy->getPosition();
    }
    else if (enemy->getGameObjectType() == GameObjectType::Building)
    {
        auto selfPosition = getPosition();

        auto building = static_cast<Building*>(enemy);
        auto& bottomGridInMapPositionList = building->getBottomGridInMapPositionList();
        float minDistance = FLT_MAX;
        for (auto& bottomGridInMapPosition : bottomGridInMapPositionList)
        {
            float distance = GameUtils::computeDistanceBetween(selfPosition, bottomGridInMapPosition);
            if (distance < minDistance)
            {
                minDistance = distance;
                arrivePosition = bottomGridInMapPosition;
            }
        }
    }

    return arrivePosition;
}

float Npc::getDistanceFrom(GameObject* enemy)
{
    auto enemyPosition = computeArrivePositionBy(enemy);
    Vec2 npcPosition = getPosition();

    float distanceResult = GameUtils::computeDistanceBetween(enemyPosition, npcPosition);

    return distanceResult;
}

RepeatForever* Npc::createAnimateWidthPList(const string& plist, float animateDelayPerUnit, NpcStatus animateType)
{
    auto animation = GameUtils::createAnimationWithPList(plist);

    auto& contentSize = getContentSize();
    if (contentSize.width == 0.0f || contentSize.height == 0.0f)
    {
        auto spriteFrame = animation->getFrames().at(0)->getSpriteFrame();
        setContentSize(spriteFrame->getOriginalSizeInPixels());
    }

    animation->setDelayPerUnit(animateDelayPerUnit);
    animation->setRestoreOriginalFrame(true);

    RepeatForever* repeatForeverAnimate = nullptr;
    switch (animateType)
    {
    case NpcStatus::Move:
    case NpcStatus::Stand:
    {
        repeatForeverAnimate = RepeatForever::create(Animate::create(animation));
    }
        break;
    case NpcStatus::Attack:
    {
        auto animationEndFunc = CallFunc::create(CC_CALLBACK_0(Npc::onAttackAnimationEnd, this));
        auto attackSequenceAction = Sequence::create(Animate::create(animation), animationEndFunc, nullptr);
        repeatForeverAnimate = RepeatForever::create(attackSequenceAction);
    }
        break;
    case NpcStatus::Die:
    {
        auto animationEndFunc = CallFunc::create(CC_CALLBACK_0(Npc::onDieAnimationEnd, this));
        auto dieSequenceAction = Sequence::create(Animate::create(animation), animationEndFunc, nullptr);
        repeatForeverAnimate = RepeatForever::create(dieSequenceAction);

        _dieAnimationFrameSize = animation->getFrames().at(0)->getSpriteFrame()->getOriginalSizeInPixels();
    }
        break;
    default:    break;
    }

    repeatForeverAnimate->retain();

    return repeatForeverAnimate;
}

void Npc::registerSwitchStatusFunction(SwitchStatusFunction& switchStatusFunctions, std::function<bool()> canSwitch, std::function<void()> switchFunction)
{
    switchStatusFunctions.canSwitch = canSwitch;
    switchStatusFunctions.switchFunction = switchFunction;
}

bool Npc::canSwitchMoveToMove()
{
    bool result = true;

    if (_gameObjectType == GameObjectType::DefenceInBuildingNpc)
    {
        result = false;
    }

    return result;
}

bool Npc::canSwitchMoveToStand()
{
    bool result = true;

    return result;
}

bool Npc::canSwitchMoveToAttack()
{
    bool result = true;

    return result;
}

bool Npc::canSwitchMoveToDie()
{
    bool result = true;

    return result;
}

void Npc::switchMoveToMove()
{
    onMoveTo();
}

void Npc::switchMoveToStand()
{
    onStand();
}

void Npc::switchMoveToAttack()
{
    onAttack();
}

void Npc::switchMoveToDie()
{
    onDie();
}

bool Npc::canSwitchStandToStand()
{
    bool result = true;

    return result;
}

bool Npc::canSwitchStandToMove()
{
    bool result = true;

    if (_gameObjectType == GameObjectType::DefenceInBuildingNpc)
    {
        result = false;
    }

    return result;
}

bool Npc::canSwitchStandToAttack()
{
    bool result = true;

    return result;
}

bool Npc::canSwitchStandToDie()
{
    bool result = true;

    return result;
}

void Npc::switchStandToStand()
{
    onStand();
}

void Npc::switchStandToMove()
{
    onMoveTo();
}

void Npc::switchStandToAttack()
{
    onAttack();
}

void Npc::switchStandToDie()
{
    onDie();
}

bool Npc::canSwitchAttackToAttack()
{
    bool result = false;

    auto enemy = GameObjectManager::getInstance()->getGameObjectBy(_enemyUniqueID);
    if (enemy)
    {
        auto newFaceToDirection = getFaceToDirection(enemy->getPosition());
        if (_faceDirection != newFaceToDirection)
        {
            _faceDirection = newFaceToDirection;
            result = true;
        }
    }

    return result;
}

bool Npc::canSwitchAttackToMove()
{
    bool result = false;

    if (_gameObjectType == GameObjectType::DefenceInBuildingNpc)
    {
        result = false;
        return result;
    }

    if (_forceType == ForceType::Player && isSelected())
    {
        result = true;
        return result;
    }

    auto attackAnimateIter = _attackAnimateMap.find(_faceDirection);
    auto attackInnerAction = attackAnimateIter->second->getInnerAction();
    auto attackAnimationElapsed = attackInnerAction->getElapsed();
    auto attackAnimationDuration = attackInnerAction->getDuration();

    if (attackAnimationElapsed >= attackAnimationDuration * 0.8f)
    {
        _gameWorld->_createBullet(_bulletType, _uniqueID, _enemyUniqueID);
        result = true;
    }

    return result;
}

bool Npc::canSwitchAttackToStand()
{
    bool result = true;

    return result;
}

bool Npc::canSwitchAttackToDie()
{
    bool result = true;

    return result;
}

void Npc::switchAttackToAttack()
{
    onAttack();
}

void Npc::switchAttackToMove()
{
    onMoveTo();
}

void Npc::switchAttackToStand()
{
    onStand();
}

void Npc::switchAttackToDie()
{
    onDie();
}

bool Npc::canSwitchDieToDie()
{
    bool result = false;

    return result;
}

bool Npc::canSwitchDieToMove()
{
    bool result = false;

    if (_gameObjectType == GameObjectType::DefenceInBuildingNpc)
    {
        result = false;
    }

    return result;
}

bool Npc::canSwitchDieToStand()
{
    bool result = false;

    return result;
}

bool Npc::canSwitchDieToAttack()
{
    bool result = false;

    return result;
}

void Npc::switchDieToDie()
{

}

void Npc::switchDieToMove()
{
    onMoveTo();
}

void Npc::switchDieToStand()
{
    onStand();
}

void Npc::switchDieToAttack()
{
    onAttack();
}

void Npc::moveTo(const Vec2& targetPosition, bool isAllowEndTileNodeToMoveIn /* = false */)
{
    _isReadyToMove = false;

    auto startPosition = getPosition();
    _gotoTargetPositionPathList.clear();
    _gotoTargetPositionPathList = _gameWorld->_computePathListBetween(startPosition, targetPosition, isAllowEndTileNodeToMoveIn);

    if (_gotoTargetPositionPathList.empty())
    {
        tryUpdateStatus(NpcStatus::Stand);
    }
    else
    {
        tryUpdateStatus(NpcStatus::Move);
    }
}

void Npc::setReadyToMoveStatus(bool isReadyToMove)
{
    _isReadyToMove = isReadyToMove;
}

NpcStatus Npc::getNpcStatus()
{
    return _oldStatus;
}

FaceDirection Npc::getFaceToDirection(const Vec2& moveToPosition)
{
    FaceDirection faceToDirection = FaceDirection::Invalid;
    auto position = getPosition();

    float degree = GameUtils::computeRotatedDegree(position, moveToPosition);
    if (degree >= 0.0f && degree < 60.0f)
    {
        faceToDirection = FaceDirection::FaceToNorthEast;
    }
    else if (degree >= 60.0f && degree < 120.0f)
    {
        faceToDirection = FaceDirection::FaceToEast;
    }
    else if (degree >= 120.0f && degree < 180.0f)
    {
        faceToDirection = FaceDirection::FaceToSouthEast;
    }
    else if (degree >= 180.0f && degree < 240.0f)
    {
        faceToDirection = FaceDirection::FaceToSouthWest;
    }
    else if (degree >= 240.0f && degree < 300.0f)
    {
        faceToDirection = FaceDirection::FaceToWest;
    }
    else
    {
        faceToDirection = FaceDirection::FaceToNorthWest;
    }

    return faceToDirection;
}

float Npc::getMoveToDuration(const Vec2& moveToPosition)
{
    auto position = getPosition();
    float distance = sqrt((moveToPosition.x - position.x) * (moveToPosition.x - position.x) +
        (moveToPosition.y - position.y) * (moveToPosition.y - position.y));

    return distance / _perSecondMoveSpeedByPixel;
}

void Npc::onMoveTo()
{
    if (_gotoTargetPositionPathList.empty())
    {
        return;
    }

    auto moveToPosition = _gotoTargetPositionPathList.front();
    _gotoTargetPositionPathList.pop_front();

    auto currentFaceDirection = getFaceToDirection(moveToPosition);
    float moveToDuration = getMoveToDuration(moveToPosition);

    auto animateIter = _moveAnimateMap.find(currentFaceDirection);
    if (animateIter != _moveAnimateMap.end())
    {
        if ((_oldStatus == NpcStatus::Move && _faceDirection != currentFaceDirection) ||
            _oldStatus != NpcStatus::Move)
        {
            stopAllActions();

            _faceDirection = currentFaceDirection;
            runAction(animateIter->second);
        }

        stopActionByTag(MOVE_TO_ACTION_TAG);

        auto positionDelta = moveToPosition - getPosition();
        auto moveBy = CustomMoveBy::create(moveToDuration, positionDelta);

        CallFunc* onMoveEndEvent = nullptr;
        if (_gotoTargetPositionPathList.empty())
        {
            onMoveEndEvent = CallFunc::create(CC_CALLBACK_0(Npc::tryUpdateStatus, this, NpcStatus::Stand));
        }
        else
        {
            onMoveEndEvent = CallFunc::create(CC_CALLBACK_0(Npc::onMoveTo, this));
        }
        auto sequenceAction = Sequence::create(moveBy, onMoveEndEvent, nullptr);
        sequenceAction->setTag(MOVE_TO_ACTION_TAG);

        runAction(sequenceAction);
    }
}

void Npc::onStand()
{
    _gotoTargetPositionPathList.clear();

    stopAllActions();

    auto standAnimateIter = _standAnimateMap.find(_faceDirection);
    if (standAnimateIter != _standAnimateMap.end())
    {
        runAction(standAnimateIter->second);
    }
}

void Npc::onAttack()
{
    _gotoTargetPositionPathList.clear();

    stopAllActions();

    auto attackAnimateIter = _attackAnimateMap.find(_faceDirection);
    if (attackAnimateIter != _attackAnimateMap.end())
    {
        runAction(attackAnimateIter->second);
    }
}

void Npc::onDie()
{
    _selectedTips->setVisible(false);

    _gotoTargetPositionPathList.clear();

    auto shadowPosition = _shadowSprite->getPosition();
    shadowPosition.x = _dieAnimationFrameSize.width / 2.0f;
    shadowPosition.y = _dieAnimationFrameSize.height / 4.0f;
    _shadowSprite->setPosition(shadowPosition);

    stopAllActions();

    runAction(_dieAnimate);
}

void Npc::onDieAnimationEnd()
{
    GameObjectManager::getInstance()->addReadyToRemoveGameObject(_uniqueID);
}

void Npc::onAttackAnimationEnd()
{
    if (_bulletType != BulletType::Invalid)
    {
        _gameWorld->_createBullet(_bulletType, _uniqueID, _enemyUniqueID);
    }
    else
    {
        auto enemy = GameObjectManager::getInstance()->getGameObjectBy(_enemyUniqueID);
        if (enemy && !enemy->isReadyToRemove())
        {
            enemy->costHP(getAttackPower());
        }
    }
}

void Npc::onPrepareToRemove()
{
    tryUpdateStatus(NpcStatus::Die);
}

list<Vec2> Npc::getPathListTo(const Vec2& inMapEndPosition)
{
    auto inMapStartPosition = getPosition();
    list<Vec2> pathList = _gameWorld->_computePathListBetween(inMapStartPosition, inMapEndPosition, false);

    return pathList;
}

const Vec2& Npc::getPosition() const
{
    if (_gameObjectType == GameObjectType::DefenceInBuildingNpc)
    {
        return _inMapPosition;
    }
    else
    {
        return GameObject::getPosition();
    }
}

void Npc::initDefenceInBuildingNpcInMapPosition()
{
    auto parentNode = getParent();
    if (!parentNode || _gameObjectType != GameObjectType::DefenceInBuildingNpc)
    {
        return;
    }

    auto worldPosition = parentNode->convertToWorldSpace(GameObject::getPosition());

    auto mapManger = GameWorldCallBackFunctionsManager::getInstance()->_getMapManager();
    _inMapPosition = mapManger->convertToTileMapSpace(worldPosition);
}

void Npc::setAlertRange(float alertRange)
{
    _maxAlertRadius = alertRange;
}

void Npc::setAttackRange(float attackRange)
{
    _maxAttackRadius = attackRange;
}

void Npc::setAttackPower(float attackPower)
{
    _attackPower = attackPower;
}