#pragma once

class GameObject;

class BulletManager
{
public:
    static BulletManager* getInstance();
    Node* createBullet(BulletType bulletType, int attackerID, int attackTargetID);
private:
    void onNormalDamageBulletMoveEnd(Node* bullet, int attackTargetID, int damageAmount);
    void onAOEDamageBulletMoveEnd(Node* bullet, GameObject* attacker, const Vec2& endPosition);

    BulletManager(){}
    BulletManager(const BulletManager&);
    BulletManager& operator = (const BulletManager&);
};