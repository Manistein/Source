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

    int maxHp = 0;
    int attackPower = 0;
    int maxAttackRadius = 0;
    int perSecondAttackCount = 0;
    AttackType attackType = AttackType::Invalid;
    DamageType damageType = DamageType::Invalid;
};

class TemplateManager
{
public:
    ~TemplateManager();

    static TemplateManager* getInstance();
    const NpcTemplate* getNpcTemplateBy(const string& jobName);
private:
    bool init();

    map<string, NpcTemplate*> _npcTemplatesMap;

    TemplateManager(){}
    TemplateManager(const TemplateManager&);
    TemplateManager& operator = (const TemplateManager&);
};