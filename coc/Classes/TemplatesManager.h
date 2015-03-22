#pragma once

struct NpcTemplate 
{
    string moveToNorthEastAnimationPList;
    string moveToEastAnimationPList;
    string moveToSouthEastAnimationPList;
    string moveToSouthWestAnimationPList;
    string moveToWestAnimationPList;
    string moveToNorthWestAnimationPList;

    string dieAnimationPList;

    string standAndFaceToNorthEastAnimationPList;
    string standAndFaceToEastAnimationPList;
    string standAndFaceToSouthEastAnimationPList;
    string standAndFaceToSouthWestAnimationPList;
    string standAndFaceToWestAnimationPList;
    string standAndFaceToNorthWestAnimationPList;

    string attackToNorthEastAnimationPList;
    string attackToEastAnimationPList;
    string attackToSouthEastAnimationPList;
    string attackToSouthWestAnimationPList;
    string attackToWestAnimationPList;
    string attackToNorthWestAnimationPList;

    int maxHp = 0;
    int attackPower = 0;
    int maxAttackRadius = 0;
    int maxAlertRadius = 0;
    int perSecondAttackCount = 0;
    BulletType bulletType = BulletType::Invalid;
    DamageType damageType = DamageType::Invalid;
};

struct BulletTemplate
{
    string bulletPList;
};

class TemplateManager
{
public:
    ~TemplateManager();

    static TemplateManager* getInstance();
    const NpcTemplate* getNpcTemplateBy(const string& jobName);
    const BulletTemplate* getBulletTemplateBy(BulletType bulletType);
private:
    bool init();
    bool initNpcTemplates();
    bool initBulletTemplates();

    map<string, NpcTemplate*> _npcTemplatesMap;
    map<BulletType, BulletTemplate*> _bulletTemplatesMap;

    TemplateManager(){}
    TemplateManager(const TemplateManager&);
    TemplateManager& operator = (const TemplateManager&);
};