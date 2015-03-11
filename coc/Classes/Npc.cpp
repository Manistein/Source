#include "Base.h"
#include "GameObject.h"
#include "Npc.h"
#include "TemplatesManager.h"
#include <math.h>

const float MOVE_ANIMATE_DELAY_PER_UNIT = 0.1f;
const float STAND_ANIMATE_DELAY_PER_UNIT = 3.0f;
const float DIE_ANIMATE_DELAY_PER_UNIT = 0.1f;

const string SHADOW_TEXTURE_NAME = "Shadow.png";
const string HP_BAR_BACKGROUND_TEXTURE_NAME = "HPBarBackground.png";
const string PLAYER_HP_BAR_TEXTURE_NAME = "PlayerHPBar.png";

const int MOVE_TO_ACTION_TAG = 1;

Npc::~Npc()
{
    clear();
}

Npc* Npc::create(const string& jobName, const Vec2& position, int uniqueID)
{
    auto npc = new Npc();
    if (npc && npc->init(jobName, position, uniqueID))
    {
        npc->autorelease();
    }
    else
    {
        CC_SAFE_DELETE(npc);
    }

    return npc;
}

bool Npc::init(const string& jobName, const Vec2& position, int uniqueID)
{
    if (!Sprite::init())
    {
        return false;
    }

    initAnimates(jobName);
    initSwitchStatusFunctions();
    initShadow();
    initHPBar();

    setPosition(position);
    _uniqueID = uniqueID;

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

    _moveAnimateMap[FaceDirection::FaceToEast] = createAnimateWidthPList(npcTempalte->moveToEastAnimationPList, MOVE_ANIMATE_DELAY_PER_UNIT);
    _moveAnimateMap[FaceDirection::FaceToNorthEast] = createAnimateWidthPList(npcTempalte->moveToNorthEastAnimationPList, MOVE_ANIMATE_DELAY_PER_UNIT);
    _moveAnimateMap[FaceDirection::FaceToNorthWest] = createAnimateWidthPList(npcTempalte->moveToNorthWestAnimationPList, MOVE_ANIMATE_DELAY_PER_UNIT);
    _moveAnimateMap[FaceDirection::FaceToSouthEast] = createAnimateWidthPList(npcTempalte->moveToSouthEastAnimationPList, MOVE_ANIMATE_DELAY_PER_UNIT);
    _moveAnimateMap[FaceDirection::FaceToSouthWest] = createAnimateWidthPList(npcTempalte->moveToSouthWestAnimationPList, MOVE_ANIMATE_DELAY_PER_UNIT);
    _moveAnimateMap[FaceDirection::FaceToWest] = createAnimateWidthPList(npcTempalte->moveToWestAnimationPList, MOVE_ANIMATE_DELAY_PER_UNIT);

    _dieAnimate = createAnimateWidthPList(npcTempalte->dieAnimationPList, DIE_ANIMATE_DELAY_PER_UNIT);

    _standAnimateMap[FaceDirection::FaceToEast] = createAnimateWidthPList(npcTempalte->standAndFaceToEastAnimationPList, STAND_ANIMATE_DELAY_PER_UNIT);
    _standAnimateMap[FaceDirection::FaceToNorthEast] = createAnimateWidthPList(npcTempalte->standAndFaceToNorthEastAnimationPList, STAND_ANIMATE_DELAY_PER_UNIT);
    _standAnimateMap[FaceDirection::FaceToNorthWest] = createAnimateWidthPList(npcTempalte->standAndFaceToNorthWestAnimationPList, STAND_ANIMATE_DELAY_PER_UNIT);
    _standAnimateMap[FaceDirection::FaceToSouthEast] = createAnimateWidthPList(npcTempalte->standAndFaceToSouthEastAnimationPList, STAND_ANIMATE_DELAY_PER_UNIT);
    _standAnimateMap[FaceDirection::FaceToSouthWest] = createAnimateWidthPList(npcTempalte->standAndFaceToSouthWestAnimationPList, STAND_ANIMATE_DELAY_PER_UNIT);
    _standAnimateMap[FaceDirection::FaceToWest] = createAnimateWidthPList(npcTempalte->standAndFaceToWestAnimationPList, STAND_ANIMATE_DELAY_PER_UNIT);

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

    _hpBar = ui::LoadingBar::create(PLAYER_HP_BAR_TEXTURE_NAME);
    _hpBar->setAnchorPoint(Vec2::ZERO);
    _hpBar->setPercent(100.0f);

    auto hpBarBackground = Sprite::create(HP_BAR_BACKGROUND_TEXTURE_NAME);
    hpBarBackground->setPosition(Vec2(contentSize.width / 2.0f, contentSize.height + _hpBar->getContentSize().height * 1.2f));
    hpBarBackground->setCascadeOpacityEnabled(true);
    hpBarBackground->setScale(0.5f);
    hpBarBackground->addChild(_hpBar);

    addChild(hpBarBackground);
}

void Npc::updateStatus(NpcStatus newStatus)
{
    auto& canSwitch = _switchStatusFunctions[(int)_oldStatus][(int)newStatus].canSwitch;
    auto& switchFunction = _switchStatusFunctions[(int)_oldStatus][(int)newStatus].switchFunction;

    if (canSwitch())
    {
        switchFunction();
        _oldStatus = newStatus;
    }
}

RepeatForever* Npc::createAnimateWidthPList(const string& plist, float animateDelayPerUnit)
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

    auto repeatForeverAnimate = RepeatForever::create(Animate::create(animation));
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
    onMoveToEvent();
}

void Npc::switchMoveToStand()
{
    onStandEvent();
}

void Npc::switchMoveToAttack()
{

}

void Npc::switchMoveToDie()
{

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
    onStandEvent();
}

void Npc::switchStandToMove()
{
    onMoveToEvent();
}

void Npc::switchStandToAttack()
{

}

void Npc::switchStandToDie()
{

}

bool Npc::canSwitchAttackToAttack()
{
    bool result = true;

    return result;
}

bool Npc::canSwitchAttackToMove()
{
    bool result = true;

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

}

void Npc::switchAttackToMove()
{
    onMoveToEvent();
}

void Npc::switchAttackToStand()
{
    onStandEvent();
}

void Npc::switchAttackToDie()
{

}

bool Npc::canSwitchDieToDie()
{
    bool result = true;

    return result;
}

bool Npc::canSwitchDieToMove()
{
    bool result = true;

    return result;
}

bool Npc::canSwitchDieToStand()
{
    bool result = true;

    return result;
}

bool Npc::canSwitchDieToAttack()
{
    bool result = true;

    return result;
}

void Npc::switchDieToDie()
{

}

void Npc::switchDieToMove()
{
    onMoveToEvent();
}

void Npc::switchDieToStand()
{
    onStandEvent();
}

void Npc::switchDieToAttack()
{

}

void Npc::moveTo(const Vec2& targetPosition)
{
    _moveToPosition = targetPosition;
    updateStatus(NpcStatus::Move);
}

void Npc::showHPBar()
{
    auto hpBarBackground = _hpBar->getParent();
    hpBarBackground->setVisible(false);
}

void Npc::hideHPBar()
{
    auto hpBarBackground = _hpBar->getParent();
    hpBarBackground->setVisible(true);
}

FaceDirection Npc::getFaceToDirection(const Vec2& moveToPosition)
{
    FaceDirection faceToDirection = FaceDirection::Invalid;

    auto contentSize = getContentSize() * getScale();
    auto position = getPosition();
    float minY = position.y - contentSize.height / 2.0f;
    float maxY = position.y + contentSize.height / 2.0f;

    if (moveToPosition.x < position.x && moveToPosition.y >= maxY)
    {
        faceToDirection = FaceDirection::FaceToNorthWest;
    }
    else if (moveToPosition.x < position.x && (moveToPosition.y < maxY && moveToPosition.y >= minY))
    {
        faceToDirection = FaceDirection::FaceToWest;
    }
    else if (moveToPosition.x < position.x && moveToPosition.y < minY)
    {
        faceToDirection = FaceDirection::FaceToSouthWest;
    }
    else if (moveToPosition.x >= position.x && moveToPosition.y >= maxY)
    {
        faceToDirection = FaceDirection::FaceToNorthEast;
    }
    else if (moveToPosition.x >= position.x && (moveToPosition.y < maxY && moveToPosition.y >= minY))
    {
        faceToDirection = FaceDirection::FaceToEast;
    }
    else
    {
        faceToDirection = FaceDirection::FaceToSouthEast;
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

void Npc::onMoveToEvent()
{
    if (_oldStatus == NpcStatus::Die)
    {
        return;
    }

    auto currentFaceDirection = getFaceToDirection(_moveToPosition);
    float moveToDuration = getMoveToDuration(_moveToPosition);

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

        auto moveTo = MoveTo::create(moveToDuration, _moveToPosition);
        auto onMoveEndEvent = CallFunc::create(CC_CALLBACK_0(Npc::updateStatus, this, NpcStatus::Stand));
        auto sequenceAction = Sequence::create(moveTo, onMoveEndEvent, nullptr);
        sequenceAction->setTag(MOVE_TO_ACTION_TAG);

        runAction(sequenceAction);
    }
}

void Npc::onStandEvent()
{
    if (_oldStatus == NpcStatus::Die)
    {
        return;
    }

    stopAllActions();

    auto standIter = _standAnimateMap.find(_faceDirection);
    if (standIter != _standAnimateMap.end())
    {
        runAction(standIter->second);
    }
}