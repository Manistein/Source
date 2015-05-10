#include "Base.h"
#include "GameObject.h"
#include "BulletManager.h"
#include "TemplatesManager.h"
#include "GameObjectManager.h"
#include "Utils.h"

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

    if (attacker && target)
    {
        switch (bulletType)
        {
        case BulletType::Arrow:
            bullet = createArrow(bulletType, attacker, target);
            break;
        default:    break;
        }
    }

    return bullet;
}

Node* BulletManager::createArrow(BulletType bulletType, GameObject* attacker, GameObject* target)
{
    auto bulletTemplate = TemplateManager::getInstance()->getBulletTemplateBy(bulletType);
    auto arrow = Sprite::create(bulletTemplate->bulletFileName);

    auto attackerPosition = attacker->getPosition();
    auto targetPosition = target->getPosition();

    arrow->setPosition(attackerPosition);

    auto rotation = GameUtils::computeRotatedDegree(attackerPosition, targetPosition);
    arrow->setRotation(rotation);
    
    auto distance = sqrt((attackerPosition.x - targetPosition.x) * (attackerPosition.x - targetPosition.x) +
        (attackerPosition.y - targetPosition.y) * (attackerPosition.y - targetPosition.y));

    auto duration = distance / ARROW_MOVE_SPEED_BY_PIXEL;
    auto moveTo = MoveTo::create(duration, targetPosition);
    auto onMoveEnd = CallFunc::create(CC_CALLBACK_0(BulletManager::onBulletMoveEnd, this, arrow, target->getUniqueID(), attacker->getAttackPower()));
    auto sequenceAction = Sequence::create(moveTo, onMoveEnd, nullptr);
    arrow->runAction(sequenceAction);

    return arrow;
}

void BulletManager::onBulletMoveEnd(Node* bullet, int attackTargetID, int damageAmount)
{
    auto attackTarget = GameObjectManager::getInstance()->getGameObjectBy(attackTargetID);
    if (attackTarget && !attackTarget->isReadyToRemove())
    {
        attackTarget->costHP(damageAmount);
    }

    bullet->retain();
    bullet->removeFromParent();
    bullet->autorelease();
}