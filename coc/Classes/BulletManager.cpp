#include "Base.h"
#include "GameObject.h"
#include "BulletManager.h"
#include "TemplatesManager.h"
#include "GameObjectManager.h"
#include "Utils.h"
#include "GameWorldCallBackFunctionsManager.h"

static BulletManager* s_bulletManager = nullptr;

const float ARROW_MOVE_SPEED_BY_PIXEL = 1000.0f;

BulletManager* BulletManager::getInstance()
{
    if (!s_bulletManager)
    {
        s_bulletManager = new BulletManager();
    }

    return s_bulletManager;
}

Node* BulletManager::createBullet(BulletType bulletType, int attackerID, int attackTargetID)
{
    Node* bullet = nullptr;

    auto attacker = GameObjectManager::getInstance()->getGameObjectBy(attackerID);
    auto target = GameObjectManager::getInstance()->getGameObjectBy(attackTargetID);

    if (attacker && target && bulletType != BulletType::Invalid)
    {
        auto bulletTemplate = TemplateManager::getInstance()->getBulletTemplateBy(bulletType);
        bullet = Sprite::create(bulletTemplate->bulletFileName);

        auto attackerPosition = attacker->getPosition();
        auto targetPosition = target->getPosition();

        if (bulletType == BulletType::Bomb)
        {
            attackerPosition.y = attackerPosition.y - attacker->getContentSize().height / 2.0f;
            targetPosition.x = attackerPosition.x;
        }

        bullet->setPosition(attackerPosition);

        auto rotation = GameUtils::computeRotatedDegree(attackerPosition, targetPosition);
        bullet->setRotation(rotation);

        auto distance = GameUtils::computeDistanceBetween(attackerPosition, targetPosition);

        auto duration = distance / ARROW_MOVE_SPEED_BY_PIXEL;
        auto moveTo = MoveTo::create(duration, targetPosition);

        CallFunc* onMoveEnd = nullptr;
        if (attacker->getDamageType() == DamageType::AreaOfEffect)
        {
            onMoveEnd = CallFunc::create(CC_CALLBACK_0(BulletManager::onAOEDamageBulletMoveEnd, this, bullet, bulletType, attacker, targetPosition));
        }
        else
        {
            onMoveEnd = CallFunc::create(
                CC_CALLBACK_0(BulletManager::onNormalDamageBulletMoveEnd, 
                this, 
                bullet, 
                bulletType, 
                target->getUniqueID(), 
                attacker->getAttackPower()));
        }

        auto sequenceAction = Sequence::create(moveTo, onMoveEnd, nullptr);
        bullet->runAction(sequenceAction);
    }

    return bullet;
}

void BulletManager::onNormalDamageBulletMoveEnd(Node* bullet, BulletType bulletType, int attackTargetID, int damageAmount)
{
    auto attackTarget = GameObjectManager::getInstance()->getGameObjectBy(attackTargetID);
    if (attackTarget && !attackTarget->isReadyToRemove())
    {
        attackTarget->costHP(damageAmount);
        onCreateSpecialEffect(bulletType, attackTarget->getPosition());
    }

    bullet->retain();
    bullet->removeFromParent();
    bullet->autorelease();
}

void BulletManager::onAOEDamageBulletMoveEnd(Node* bullet, BulletType bulletType, GameObject* attacker, const Vec2& endPosition)
{
    auto attackerForceType = attacker->getForceType();
    auto attackerAoeDamageRadius = attacker->getAoeDamageRadius();
    auto attackerDamagePower = attacker->getAttackPower();

    auto gameObjectsMap = GameObjectManager::getInstance()->getGameObjectMap();

    vector<int> attackedTargetUniqueIDList;
    for (auto& gameObjectIter : gameObjectsMap)
    {
        auto gameObject = gameObjectIter.second;
        if (gameObject->isReadyToRemove() ||
            gameObject->getForceType() == attackerForceType ||
            gameObject->getGameObjectType() == GameObjectType::DefenceInBuildingNpc)
        {
            continue;
        }

        auto gameObjectPosition = gameObject->getPosition();
        auto distance = GameUtils::computeDistanceBetween(endPosition, gameObjectPosition);
        if (distance <= attackerAoeDamageRadius)
        {
            attackedTargetUniqueIDList.push_back(gameObject->getUniqueID());
        }
    }

    for (auto uniqueID : attackedTargetUniqueIDList)
    {
        auto gameObjectIter = gameObjectsMap.find(uniqueID);
        gameObjectIter->second->costHP(attackerDamagePower);
    }

    onCreateSpecialEffect(bulletType, endPosition);

    bullet->retain();
    bullet->removeFromParent();
    bullet->autorelease();
}

void BulletManager::onCreateSpecialEffect(BulletType bulletType, const Vec2& inMapPosition)
{
    auto bulletTemplate = TemplateManager::getInstance()->getBulletTemplateBy(bulletType);
    if (bulletTemplate)
    {
        GameWorldCallBackFunctionsManager::getInstance()->_createSpecialEffect(bulletTemplate->specialEffectTemplateName, inMapPosition, false);
    }
}
