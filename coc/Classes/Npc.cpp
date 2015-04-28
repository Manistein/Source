#include "Base.h"
#include "GameObject.h"
#include "Npc.h"
#include "TemplatesManager.h"
#include <math.h>
#include "GameObjectManager.h"
#include "GameSetting.h"
#include "GameWorldCallBackFunctionsManager.h"
#include "Utils.h"

const float MOVE_ANIMATE_DELAY_PER_UNIT = 0.1f;
const float STAND_ANIMATE_DELAY_PER_UNIT = 3.0f;
const float DIE_ANIMATE_DELAY_PER_UNIT = 0.5f;
const float ATTACK_ANIMATE_DELAY_PER_UNIT = 0.15f;

const string SHADOW_TEXTURE_NAME = "Shadow.png";
const string SHOW_WORLD_POSITION_CHILD_NAME = "ShowWorldPositionChildName";

const int MOVE_TO_ACTION_TAG = 1;

Npc::~Npc()
{
    clear();
}

Npc* Npc::create(ForceType forceType, const string& jobName, const Vec2& position, int uniqueID)
{
    auto npc = new Npc();
    if (npc && npc->init(forceType, jobName, position, uniqueID))
    {
        npc->autorelease();
    }
    else
    {
        CC_SAFE_DELETE(npc);
    }

    return npc;
}

bool Npc::init(ForceType forceType, const string& jobName, const Vec2& position, int uniqueID)
{
    _forceType = forceType;

    if (!GameObject::init())
    {
        return false;
    }

    initAnimates(jobName);
    initSwitchStatusFunctions();
    initShadow();
    initHPBar();
    initBattleData(jobName);
    initDebugDraw();

    setPosition(position);
    _uniqueID = uniqueID;

    _gameObjectType = GameObjectType::Npc;

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

    _moveAnimateMap[FaceDirection::FaceToEast] = createAnimateWidthPList(npcTempalte->moveToEastAnimationPList, MOVE_ANIMATE_DELAY_PER_UNIT, NpcStatus::Move);
    _moveAnimateMap[FaceDirection::FaceToNorthEast] = createAnimateWidthPList(npcTempalte->moveToNorthEastAnimationPList, MOVE_ANIMATE_DELAY_PER_UNIT, NpcStatus::Move);
    _moveAnimateMap[FaceDirection::FaceToNorthWest] = createAnimateWidthPList(npcTempalte->moveToNorthWestAnimationPList, MOVE_ANIMATE_DELAY_PER_UNIT, NpcStatus::Move);
    _moveAnimateMap[FaceDirection::FaceToSouthEast] = createAnimateWidthPList(npcTempalte->moveToSouthEastAnimationPList, MOVE_ANIMATE_DELAY_PER_UNIT, NpcStatus::Move);
    _moveAnimateMap[FaceDirection::FaceToSouthWest] = createAnimateWidthPList(npcTempalte->moveToSouthWestAnimationPList, MOVE_ANIMATE_DELAY_PER_UNIT, NpcStatus::Move);
    _moveAnimateMap[FaceDirection::FaceToWest] = createAnimateWidthPList(npcTempalte->moveToWestAnimationPList, MOVE_ANIMATE_DELAY_PER_UNIT, NpcStatus::Move);

    _dieAnimate = createAnimateWidthPList(npcTempalte->dieAnimationPList, DIE_ANIMATE_DELAY_PER_UNIT, NpcStatus::Die);

    _standAnimateMap[FaceDirection::FaceToEast] = createAnimateWidthPList(npcTempalte->standAndFaceToEastAnimationPList, STAND_ANIMATE_DELAY_PER_UNIT, NpcStatus::Stand);
    _standAnimateMap[FaceDirection::FaceToNorthEast] = createAnimateWidthPList(npcTempalte->standAndFaceToNorthEastAnimationPList, STAND_ANIMATE_DELAY_PER_UNIT, NpcStatus::Stand);
    _standAnimateMap[FaceDirection::FaceToNorthWest] = createAnimateWidthPList(npcTempalte->standAndFaceToNorthWestAnimationPList, STAND_ANIMATE_DELAY_PER_UNIT, NpcStatus::Stand);
    _standAnimateMap[FaceDirection::FaceToSouthEast] = createAnimateWidthPList(npcTempalte->standAndFaceToSouthEastAnimationPList, STAND_ANIMATE_DELAY_PER_UNIT, NpcStatus::Stand);
    _standAnimateMap[FaceDirection::FaceToSouthWest] = createAnimateWidthPList(npcTempalte->standAndFaceToSouthWestAnimationPList, STAND_ANIMATE_DELAY_PER_UNIT, NpcStatus::Stand);
    _standAnimateMap[FaceDirection::FaceToWest] = createAnimateWidthPList(npcTempalte->standAndFaceToWestAnimationPList, STAND_ANIMATE_DELAY_PER_UNIT, NpcStatus::Stand);

    _attackAnimateMap[FaceDirection::FaceToEast] = createAnimateWidthPList(npcTempalte->attackToEastAnimationPList, ATTACK_ANIMATE_DELAY_PER_UNIT, NpcStatus::Attack);
    _attackAnimateMap[FaceDirection::FaceToNorthEast] = createAnimateWidthPList(npcTempalte->attackToNorthEastAnimationPList, ATTACK_ANIMATE_DELAY_PER_UNIT, NpcStatus::Attack);
    _attackAnimateMap[FaceDirection::FaceToNorthWest] = createAnimateWidthPList(npcTempalte->attackToNorthWestAnimationPList, ATTACK_ANIMATE_DELAY_PER_UNIT, NpcStatus::Attack);
    _attackAnimateMap[FaceDirection::FaceToSouthEast] = createAnimateWidthPList(npcTempalte->attackToSouthEastAnimationPList, ATTACK_ANIMATE_DELAY_PER_UNIT, NpcStatus::Attack);
    _attackAnimateMap[FaceDirection::FaceToSouthWest] = createAnimateWidthPList(npcTempalte->attackToSouthWestAnimationPList, ATTACK_ANIMATE_DELAY_PER_UNIT, NpcStatus::Attack);
    _attackAnimateMap[FaceDirection::FaceToWest] = createAnimateWidthPList(npcTempalte->attackToWestAnimationPList, ATTACK_ANIMATE_DELAY_PER_UNIT, NpcStatus::Attack);

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

void Npc::initShadow()
{
    auto contentSize = getContentSize();
    auto position = getPosition();

    auto shadowSprite = Sprite::create(SHADOW_TEXTURE_NAME);
    shadowSprite->setPosition(Vec2(contentSize.width / 2.0f, contentSize.height / 4.0f));
    shadowSprite->setScale(2.0f);
    addChild(shadowSprite, -1);
}

void Npc::initHPBar()
{
    auto contentSize = getContentSize();
    auto position = getPosition();

    auto hpBarBackground = _hpBar->getParent();
    hpBarBackground->setPosition(Vec2(contentSize.width / 2.0f, contentSize.height + _hpBar->getContentSize().height * 1.2f));
}

void Npc::initBattleData(const string& jobName)
{
    auto npcTemplate = TemplateManager::getInstance()->getNpcTemplateBy(jobName);
    _maxHp = npcTemplate->maxHp;
    _hp = _maxHp;
    _attackPower = npcTemplate->attackPower;
    _maxAttackRadius = npcTemplate->maxAttackRadius;
    _maxAlertRadius = npcTemplate->maxAlertRadius;
    _perSecondAttackCount = npcTemplate->perSecondAttackCount;
    _bulletType = npcTemplate->bulletType;
    _damageType = npcTemplate->damageType;
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
}

void Npc::debugDraw()
{
    _debugDrawNode->clear();

    _debugDrawNode->setVisible(true);
    _debugDrawNode->drawCircle(Vec2::ZERO, _maxAttackRadius, CC_DEGREES_TO_RADIANS(360), 50, true, 1.0f, 1.0f, Color4F(1.0f, 0.0f, 0.0f, 0.5f));
    _debugDrawNode->drawCircle(Vec2::ZERO, _maxAlertRadius, CC_DEGREES_TO_RADIANS(360), 50, true, 1.0f, 1.0f, Color4F(1.0f, 1.0f, 1.0f, 0.5f));

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
}

void Npc::clearDebugDraw()
{
    _debugDrawNode->clear();
    _debugDrawNode->setVisible(false);
}

void Npc::update(float delta)
{
    if (g_setting.allowDebugDraw)
    {
        debugDraw();
    }

    updateHP();
    runFightWithEnemyAI(delta);
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

void Npc::runFightWithEnemyAI(float delta)
{
    if (_oldStatus == NpcStatus::Die)
    {
        return;
    }

    if (_enemyUniqueID != ENEMY_UNIQUE_ID_INVALID)
    {
        auto enemy = GameObjectManager::getInstance()->getGameObjectBy(_enemyUniqueID);
        if (!enemy || enemy->isDying())
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
                if (_coolDownTimeInMoveStatus >= COOL_DOWN_TIME_IN_MOVE_STATUS_INTERVAL)
                {
                    auto npcPosition = getPosition();
                    auto enemyPosition = enemy->getPosition();

                    if (_gotoTargetPositionPathList.empty() ||
                        !_gotoTargetPositionPathList.empty() && _gotoTargetPositionPathList.back() != enemyPosition)
                    {
                        _gotoTargetPositionPathList.clear();
                        _gotoTargetPositionPathList = _gameWorld->_computePathListBetween(npcPosition, enemyPosition);

                        tryUpdateStatus(NpcStatus::Move);
                    }

                    _coolDownTimeInMoveStatus = 0.0f;
                }
            }
                break;
            case NpcStatus::Stand:
            case NpcStatus::Attack:
            {
                auto npcPosition = getPosition();
                auto enemyPosition = enemy->getPosition();

                _gotoTargetPositionPathList.clear();
                _gotoTargetPositionPathList = _gameWorld->_computePathListBetween(npcPosition, enemyPosition);

                tryUpdateStatus(NpcStatus::Move);
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
        if (_forceType == ForceType::Player && _oldStatus == NpcStatus::Move ||
            _isReadyToMove)
        {
            return;
        }

        auto gameObjectMap = GameObjectManager::getInstance()->getGameObjectMap();
        for (auto& gameObjectIter : gameObjectMap)
        {
            if (_forceType == gameObjectIter.second->getForceType() || gameObjectIter.second->isDying())
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
}

bool Npc::isEnemyInAttackRange(GameObject* enemy)
{
    bool result = false;

    float distance = getDistanceFrom(enemy);

    if (distance <= _maxAttackRadius)
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

bool Npc::isDying()
{
    return _oldStatus == NpcStatus::Die;
}

float Npc::getDistanceFrom(GameObject* enemy)
{
    auto enemyPosition = enemy->getPosition();
    auto position = getPosition();

    float distance = sqrt((enemyPosition.x - position.x) * (enemyPosition.x - position.x) +
        (enemyPosition.y - position.y) * (enemyPosition.y - position.y));

    return distance;
}

RepeatForever* Npc::createAnimateWidthPList(const string& plist, float animateDelayPerUnit, NpcStatus animateType)
{
    auto fileUtils = FileUtils::getInstance();
    CCASSERT(fileUtils->isFileExist(plist), StringUtils::format("%s is invalid", plist.c_str()).c_str());

    auto spriteFrameCache = SpriteFrameCache::getInstance();
    spriteFrameCache->addSpriteFramesWithFile(plist);

    auto animation = Animation::create();

    auto plistDataMap = fileUtils->getValueMapFromFile(plist);
    auto framesIter = plistDataMap.find("frames");
    CCASSERT(framesIter != plistDataMap.end(), StringUtils::format("frames key is invalid in %s", plist.c_str()).c_str());

    auto framesMap = plistDataMap["frames"].asValueMap();
    for (auto& iter : framesMap)
    {
        auto spriteFrame = spriteFrameCache->getSpriteFrameByName(iter.first);
        if (spriteFrame)
        {
            auto& contentSize = getContentSize();
            if (contentSize.width == 0.0f || contentSize.height == 0.0f)
            {
                setContentSize(spriteFrame->getOriginalSizeInPixels());
            }

            animation->addSpriteFrame(spriteFrame);
        }
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

    if (_forceType == ForceType::Player)
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

void Npc::moveTo(const Vec2& targetPosition)
{
    _isReadyToMove = false;

    if (_gotoTargetPositionPathList.empty() || 
        !_gotoTargetPositionPathList.empty() && _gotoTargetPositionPathList.back() != targetPosition)
    {
        auto startPosition = getPosition();
        _gotoTargetPositionPathList.clear();
        _gotoTargetPositionPathList = _gameWorld->_computePathListBetween(startPosition, targetPosition);

        if (_gotoTargetPositionPathList.empty())
        {
            tryUpdateStatus(NpcStatus::Stand);
        }
        else
        {
            tryUpdateStatus(NpcStatus::Move);
        }
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

        auto moveTo = MoveTo::create(moveToDuration, moveToPosition);

        CallFunc* onMoveEndEvent = nullptr;
        if (_gotoTargetPositionPathList.empty())
        {
            onMoveEndEvent = CallFunc::create(CC_CALLBACK_0(Npc::tryUpdateStatus, this, NpcStatus::Stand));
        }
        else
        {
            onMoveEndEvent = CallFunc::create(CC_CALLBACK_0(Npc::onMoveTo, this));
        }
        
        auto sequenceAction = Sequence::create(moveTo, onMoveEndEvent, nullptr);
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
    _gotoTargetPositionPathList.clear();

    stopAllActions();

    runAction(_dieAnimate);
}

void Npc::onDieAnimationEnd()
{
    GameObjectManager::getInstance()->addDecimatedGameObject(_uniqueID);
}

void Npc::onAttackAnimationEnd()
{
    if (_bulletType != BulletType::Invalid)
    {
        _gameWorld->_createBullet(_bulletType, _uniqueID, _enemyUniqueID);
    }
    else
    {

    }
}

void Npc::onPrepareToDestory()
{
    tryUpdateStatus(NpcStatus::Die);
}

list<Vec2> Npc::getPathListTo(const Vec2& inMapEndPosition)
{
    auto inMapStartPosition = getPosition();
    list<Vec2> pathList = _gameWorld->_computePathListBetween(inMapStartPosition, inMapEndPosition);

    return pathList;
}
