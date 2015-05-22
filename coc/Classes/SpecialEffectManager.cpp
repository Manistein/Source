#include "Base.h"
#include "GameObject.h"
#include "SpecialEffectManager.h"
#include "TemplatesManager.h"
#include "Utils.h"

static SpecialEffectManager* s_specialEffectManager = nullptr;

SpecialEffectManager* SpecialEffectManager::getInstance()
{
    if (s_specialEffectManager == nullptr)
    {
        s_specialEffectManager = new SpecialEffectManager();
        s_specialEffectManager->init();
    }

    return s_specialEffectManager;
}

bool SpecialEffectManager::init()
{
    return true;
}

Sprite* SpecialEffectManager::createSpecialEffect(const string& templateName, const Vec2& inMapPosition, bool isRepeat)
{
    auto specialEffectTemplate = TemplateManager::getInstance()->getSpecialEffectTemplateBy(templateName);

    auto effectSprite = Sprite::create();
    effectSprite->setPosition(inMapPosition);

    auto animation = GameUtils::createAnimationWithPList(specialEffectTemplate->animationPListName);
    animation->setRestoreOriginalFrame(isRepeat);
    animation->setDelayPerUnit(specialEffectTemplate->perUnitIntervalBySecond);
    auto animate = Animate::create(animation);

    auto removeSpecialEffect = CallFunc::create(CC_CALLBACK_0(SpecialEffectManager::onRemoveSpecialEffect, this, effectSprite));

    auto sequenceAction = Sequence::create(animate, removeSpecialEffect, nullptr);
    effectSprite->runAction(sequenceAction);

    return effectSprite;
}

void SpecialEffectManager::onRemoveSpecialEffect(Node* specialEffect)
{
    specialEffect->retain();
    specialEffect->removeFromParent();
    specialEffect->autorelease();
}
