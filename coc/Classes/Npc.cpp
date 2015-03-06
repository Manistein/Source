#include "Base.h"
#include "GameObject.h"
#include "Npc.h"
#include "TemplatesManager.h"

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

    auto npcTempalte = TemplateManager::getInstance()->getNpcTemplateBy(jobName);
    CCASSERT(npcTempalte, StringUtils::format("%s is not a npc type", jobName.c_str()).c_str());

    _moveToEastAnimate = createAnimateWidthPList(npcTempalte->moveToEastAnimationPList);
    _moveToNorthEastAnimate = createAnimateWidthPList(npcTempalte->moveToNorthEastAnimationPList);
    _moveToNorthWestAnimate = createAnimateWidthPList(npcTempalte->moveToNorthWestAnimationPList);
    _moveToSouthEastAnimate = createAnimateWidthPList(npcTempalte->moveToSouthEastAnimationPList);
    _moveToSouthWestAnimate = createAnimateWidthPList(npcTempalte->moveToSouthWestAnimationPList);
    _moveToWestAnimate = createAnimateWidthPList(npcTempalte->moveToWestAnimationPList);
    //_dieAnimate = createAnimateWidthPList(npcTempalte->dieAnimationPList);
    //_standAnimate = createAnimateWidthPList(npcTempalte->standAnimationPList);

    setPosition(position);
    _uniqueID = uniqueID;

    return true;
}

void Npc::clear()
{
    if (_moveToEastAnimate)
    {
        CC_SAFE_RELEASE(_moveToEastAnimate);
    }

    if (_moveToNorthEastAnimate)
    {
        CC_SAFE_RELEASE(_moveToNorthEastAnimate);
    }

    if (_moveToNorthWestAnimate)
    {
        CC_SAFE_RELEASE(_moveToNorthWestAnimate);
    }

    if (_moveToSouthEastAnimate)
    {
        CC_SAFE_RELEASE(_moveToSouthEastAnimate);
    }

    if (_moveToSouthWestAnimate)
    {
        CC_SAFE_RELEASE(_moveToSouthWestAnimate);
    }

    if (_moveToWestAnimate)
    {
        CC_SAFE_RELEASE(_moveToWestAnimate);
    }

    if (_dieAnimate)
    {
        CC_SAFE_RELEASE(_dieAnimate);
    }

    if (_standAnimate)
    {
        CC_SAFE_RELEASE(_standAnimate);
    }
}

RepeatForever* Npc::createAnimateWidthPList(const string& plist)
{
    auto fileUtils = FileUtils::getInstance();
    CCASSERT(fileUtils->isFileExist(plist), StringUtils::format("%s is invalid file name", plist.c_str()).c_str());

    auto spriteFrameCache = SpriteFrameCache::getInstance();
    spriteFrameCache->addSpriteFramesWithFile(plist);

    auto animation = Animation::create();

    auto dictMap = fileUtils->getValueMapFromFile(plist);
    auto framesIter = dictMap.find("frames");
    CCASSERT(framesIter != dictMap.end(), StringUtils::format("frames key is invalid in %s", plist.c_str()).c_str());

    auto framesMap = dictMap["frames"].asValueMap();
    for (auto& iter : framesMap)
    {
        auto spriteFrame = spriteFrameCache->getSpriteFrameByName(iter.first);
        if (spriteFrame)
        {
            animation->addSpriteFrame(spriteFrame);
        }
    }

    animation->setDelayPerUnit(0.2f);
    animation->setRestoreOriginalFrame(true);

    auto repeatForeverAnimate = RepeatForever::create(Animate::create(animation));
    repeatForeverAnimate->retain();

    return repeatForeverAnimate;
}
