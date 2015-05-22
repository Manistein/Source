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
    float perSecondMoveSpeedByPixel = 0.0f;
    BulletType bulletType = BulletType::Invalid;
    DamageType damageType = DamageType::Invalid;

    float shadowYPosition = 0.0f;
    float hpBarYPosition = 0.0f;

    float moveAnimateDelayPerUnit = 0.0f;
    float standAnimateDelayPerUnit = 0.0f;
    float dieAnimateDelayPerUnit = 0.0f;
};

struct BulletTemplate
{
    string bulletFileName;
};

struct BuildingTemplate
{
    string prepareToBuildStatusTextureName;
    string beingBuiltStatusTextureName;
    string workingStatusTextureName;
    string destroyStatusTextureName;
    string shadowTextureName;
    string destroySpecialEffectTemplateName;
    float shadowYPositionInBeingBuiltStatus = 0.0f;
    float shadowYPositionInWorkingStatus = 0.0f;
    float shadowYPositionInDestroyStatus = 0.0f;
    float buildingTimeBySecond = 0.0f;
    float extraEnemyAttackRadius = 0.0f;
    int maxHP = 0;
    float centerBottomGridYPosition = 0.0f;
};

struct SpecialEffectTemplate
{
    string animationPListName;
    float perUnitIntervalBySecond = 0.0f;
};

class TemplateManager
{
public:
    ~TemplateManager();

    static TemplateManager* getInstance();
    const NpcTemplate* getNpcTemplateBy(const string& jobName);
    const BulletTemplate* getBulletTemplateBy(BulletType bulletType);
    const BuildingTemplate* getBuildingTemplateBy(const string& buildingTemplateName);
    const SpecialEffectTemplate* getSpecialEffectTemplateBy(const string& templateName);
private:
    bool init();
    bool initNpcTemplates();
    bool initBulletTemplates();
    bool initBuildingTemplates();
    bool initSpecialEffectTemplates();

    map<string, NpcTemplate*> _npcTemplatesMap;
    map<BulletType, BulletTemplate*> _bulletTemplatesMap;
    map<string, BuildingTemplate*> _buildingTemplatesMap;
    map<string, SpecialEffectTemplate*> _specialEffectTemplatesMap;

    TemplateManager(){}
    TemplateManager(const TemplateManager&);
    TemplateManager& operator = (const TemplateManager&);
};