#include "Base.h"
#include "GameObject.h"
#include "BulletManager.h"
#include "TemplatesManager.h"
#include "GameObjectManager.h"

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

    auto rotation = computeArrowRotation(attackerPosition, targetPosition);
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

float BulletManager::computeArrowRotation(const Vec2& attackerPosition, const Vec2& targetPosition)
{
    float rotation = 0.0f;

    auto moveVector = targetPosition - attackerPosition;
    if (moveVector.x > -FLT_EPSILON && moveVector.x < FLT_EPSILON)
    {
        if (moveVector.y >= 0.0f)
        {
            rotation = 0.0f;
        }
        else
        {
            rotation = 180.0f;
        }

        return rotation;
    }
    else if (moveVector.y > -FLT_EPSILON && moveVector.y < FLT_EPSILON)
    {
        if (moveVector.x >= 0.0f)
        {
            rotation = 90.0f;
        }
        else
        {
            rotation = 270.0f;
        }

        return rotation;
    }

    float tanTheta = moveVector.x / moveVector.y;
    if (tanTheta > 0.0f)
    {
        if (moveVector.y > 0.0f)
        {
            rotation = atan(tanTheta) * 180.0f / M_PI;
        }
        else
        {
            rotation = atan(tanTheta) * 180.0f / M_PI + 180.0f;
        }
    }
    else
    {
        if (moveVector.x > 0.0f)
        {
            rotation = atan(tanTheta) * 180.0f / M_PI + 180.0f;
        }
        else
        {
            rotation = atan(tanTheta) * 180.0f / M_PI + 360.0f;
        }
    }

    return rotation;
}

void BulletManager::onBulletMoveEnd(Node* bullet, int attackTargetID, int damageAmount)
{
    auto attackTarget = GameObjectManager::getInstance()->getGameObjectBy(attackTargetID);
    if (attackTarget)
    {
        attackTarget->costHP(damageAmount);
    }

    bullet->retain();
    bullet->removeFromParent();
    bullet->autorelease();
}