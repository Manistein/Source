#pragma once

class GameObject;

class BulletManager
{
public:
    static BulletManager* getInstance();
    Node* createBullet(BulletType bulletType, int attackerID, int attackTargetID);
private:
    void onNormalDamageBulletMoveEnd(Node* bullet, BulletType bulletType, int attackTargetID, int damageAmount);
    void onAOEDamageBulletMoveEnd(Node* bullet, BulletType bulletType, GameObject* attacker, const Vec2& endPosition);

    void onCreateSpecialEffect(BulletType bulletType, const Vec2& inMapPosition);

    BulletManager(){}
    BulletManager(const BulletManager&);
    BulletManager& operator = (const BulletManager&);
};