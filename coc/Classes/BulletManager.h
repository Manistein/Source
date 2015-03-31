#pragma once

class GameObject;

class BulletManager
{
public:
    static BulletManager* getInstance();
    Node* createBullet(BulletType bulletType, int attackerID, int attackTargetID);
private:
    Node* createArrow(BulletType bulletType, GameObject* attacker, GameObject* target);
    float computeArrowRotation(const Vec2& attackerPosition, const Vec2& targetPosition);

    void onBulletMoveEnd(Node* bullet, int attackTargetID, int damageAmount);

    BulletManager(){}
    BulletManager(const BulletManager&);
    BulletManager& operator = (const BulletManager&);
};