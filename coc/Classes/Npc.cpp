#include "Base.h"
#include "GameObject.h"
#include "Npc.h"
#include "TemplatesManager.h"
#include <math.h>

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

    _moveAnimateMap[FaceDirection::FaceToEast] = createAnimateWidthPList(npcTempalte->moveToEastAnimationPList);
    _moveAnimateMap[FaceDirection::FaceToNorthEast] = createAnimateWidthPList(npcTempalte->moveToNorthEastAnimationPList);
    _moveAnimateMap[FaceDirection::FaceToNorthWest] = createAnimateWidthPList(npcTempalte->moveToNorthWestAnimationPList);
    _moveAnimateMap[FaceDirection::FaceToSouthEast] = createAnimateWidthPList(npcTempalte->moveToSouthEastAnimationPList);
    _moveAnimateMap[FaceDirection::FaceToSouthWest] = createAnimateWidthPList(npcTempalte->moveToSouthWestAnimationPList);
    _moveAnimateMap[FaceDirection::FaceToWest] = createAnimateWidthPList(npcTempalte->moveToWestAnimationPList);

    _dieAnimate = createAnimateWidthPList(npcTempalte->dieAnimationPList);

    _standAnimateMap[FaceDirection::FaceToEast] = createAnimateWidthPList(npcTempalte->standAndFaceToEastAnimationPList);
    _standAnimateMap[FaceDirection::FaceToNorthEast] = createAnimateWidthPList(npcTempalte->standAndFaceToNorthEastAnimationPList);
    _standAnimateMap[FaceDirection::FaceToNorthWest] = createAnimateWidthPList(npcTempalte->standAndFaceToNorthWestAnimationPList);
    _standAnimateMap[FaceDirection::FaceToSouthEast] = createAnimateWidthPList(npcTempalte->standAndFaceToSouthEastAnimationPList);
    _standAnimateMap[FaceDirection::FaceToSouthWest] = createAnimateWidthPList(npcTempalte->standAndFaceToSouthWestAnimationPList);
    _standAnimateMap[FaceDirection::FaceToWest] = createAnimateWidthPList(npcTempalte->standAndFaceToWestAnimationPList);

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

void Npc::updateStatus(NpcStatus newStatus)
{
    auto& canSwitch = _switchStatusFunctions[(int)_oldStatus][(int)newStatus].canSwitch;
    auto& switchFunction = _switchStatusFunctions[(int)_oldStatus][(int)newStatus].switchFunction;

    if (canSwitch())
    {
        switchFunction();
    }

    _oldStatus = newStatus;
}

RepeatForever* Npc::createAnimateWidthPList(const string& plist)
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
            animation->addSpriteFrame(spriteFrame);
        }
    }

    animation->setDelayPerUnit(0.1f);
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

void Npc::depthSort(const Size& tileSize)
{
    auto position = getPosition();
    position = CC_POINT_POINTS_TO_PIXELS(position);
    float newZ = -(position.y + tileSize.height / 3.0f) / (tileSize.height / 2.0f);
    setPositionZ(newZ);
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
    stopAllActions();

    _faceDirection = getFaceToDirection(_moveToPosition);
    float moveToDuration = getMoveToDuration(_moveToPosition);

    auto animateIter = _moveAnimateMap.find(_faceDirection);
    if (animateIter != _moveAnimateMap.end())
    {
        runAction(animateIter->second);

        auto moveTo = MoveTo::create(moveToDuration, _moveToPosition);
        auto onMoveEndEvent = CallFunc::create(CC_CALLBACK_0(Npc::onStandEvent, this));
        auto sequenceAction = Sequence::create(moveTo, onMoveEndEvent, nullptr);

        runAction(sequenceAction);
    }
}

void Npc::onStandEvent()
{
    stopAllActions();

    auto standIter = _standAnimateMap.find(_faceDirection);
    if (standIter != _standAnimateMap.end())
    {
        runAction(standIter->second);
    }
}
