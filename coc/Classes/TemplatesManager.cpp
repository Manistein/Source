#include "Base.h"
#include "GameObject.h"
#include "TemplatesManager.h"
#include "TabFileReader.h"
#include <unordered_map>

static TemplateManager* s_templateManager = nullptr;

static unordered_map<string, BulletType> s_bulletTypeStringToEnum = {
    { "Invalid", BulletType::Invalid },
    { "Arrow", BulletType::Arrow },
    { "Magic", BulletType::Magic },
};

static unordered_map<string, DamageType> s_damageTypeStringToEnum = {
    { "AreaOfEffect", DamageType::AreaOfEffect },
    { "Normal", DamageType::Normal },
};

TemplateManager::~TemplateManager()
{
    for (auto& iter : _npcTemplatesMap)
    {
        if (iter.second)
        {
            CC_SAFE_DELETE(iter.second);
        }
    }
    _npcTemplatesMap.clear();

    for (auto& iter : _bulletTemplatesMap)
    {
        if (iter.second)
        {
            CC_SAFE_DELETE(iter.second);
        }
    }
    _bulletTemplatesMap.clear();

    for (auto& iter : _buildingTemplatesMap)
    {
        if (iter.second)
        {
            CC_SAFE_DELETE(iter.second);
        }
    }
    _buildingTemplatesMap.clear();

    for (auto& iter : _specialEffectTemplatesMap)
    {
        if (iter.second)
        {
            CC_SAFE_DELETE(iter.second);
        }
    }
    _specialEffectTemplatesMap.clear();
}

TemplateManager* TemplateManager::getInstance()
{
    if (s_templateManager == nullptr)
    {
        s_templateManager = new TemplateManager();
        s_templateManager->init();
    }

    return s_templateManager;
}

const NpcTemplate* TemplateManager::getNpcTemplateBy(const string& jobName)
{
    NpcTemplate* npcTemplate = nullptr;

    auto iter = _npcTemplatesMap.find(jobName);
    if (iter != _npcTemplatesMap.end())
    {
        npcTemplate = iter->second;
    }

    return npcTemplate;
}

const BulletTemplate* TemplateManager::getBulletTemplateBy(BulletType bulletType)
{
    BulletTemplate* bulletTemplate = nullptr;

    auto iter = _bulletTemplatesMap.find(bulletType);
    if (iter != _bulletTemplatesMap.end())
    {
        bulletTemplate = iter->second;
    }

    return bulletTemplate;
}

const BuildingTemplate* TemplateManager::getBuildingTemplateBy(const string& buildingTemplateName)
{
    BuildingTemplate* buildingTemplate = nullptr;

    auto iter = _buildingTemplatesMap.find(buildingTemplateName);
    if (iter != _buildingTemplatesMap.end())
    {
        buildingTemplate = iter->second;
    }

    return buildingTemplate;
}

const SpecialEffectTemplate* TemplateManager::getSpecialEffectTemplateBy(const string& templateName)
{
    SpecialEffectTemplate* specialEffectTemplate = nullptr;

    auto iter = _specialEffectTemplatesMap.find(templateName);
    if (iter != _specialEffectTemplatesMap.end())
    {
        specialEffectTemplate = iter->second;
    }

    return specialEffectTemplate;
}

bool TemplateManager::init()
{
    bool result = false;

    result = initNpcTemplates();
    result = initBulletTemplates();
    result = initBuildingTemplates();
    result = initSpecialEffectTemplates();

    return result;
}

bool TemplateManager::initNpcTemplates()
{
    bool result = false;

    TabFileReader tabFileReader;
    if (tabFileReader.open("NpcTemplates.tab"))
    {
        for (int i = 0; i < tabFileReader.getRowCount(); i++)
        {
            auto templateName = tabFileReader.getString(i, "TemplateName");

            NpcTemplate* npcTemplate = new NpcTemplate();
            npcTemplate->moveToNorthEastAnimationPList = tabFileReader.getString(i, "MoveToNorthEastAnimationPList");
            npcTemplate->moveToEastAnimationPList = tabFileReader.getString(i, "MoveToEastAnimationPList");
            npcTemplate->moveToSouthEastAnimationPList = tabFileReader.getString(i, "MoveToSouthEastAnimationPList");
            npcTemplate->moveToSouthWestAnimationPList = tabFileReader.getString(i, "MoveToSouthWestAnimationPList");
            npcTemplate->moveToWestAnimationPList = tabFileReader.getString(i, "MoveToWestAnimationPList");
            npcTemplate->moveToNorthWestAnimationPList = tabFileReader.getString(i, "MoveToNorthWestAnimationPList");

            npcTemplate->dieAnimationPList = tabFileReader.getString(i, "DieAnimationPList");

            npcTemplate->standAndFaceToEastAnimationPList = tabFileReader.getString(i, "StandAndFaceToEastPList");
            npcTemplate->standAndFaceToNorthEastAnimationPList = tabFileReader.getString(i, "StandAndFaceToNorthEastPList");
            npcTemplate->standAndFaceToNorthWestAnimationPList = tabFileReader.getString(i, "StandAndFaceToNorthWestPList");
            npcTemplate->standAndFaceToSouthEastAnimationPList = tabFileReader.getString(i, "StandAndFaceToSouthEastPList");
            npcTemplate->standAndFaceToSouthWestAnimationPList = tabFileReader.getString(i, "StandAndFaceToSouthWestPList");
            npcTemplate->standAndFaceToWestAnimationPList = tabFileReader.getString(i, "StandAndFaceToWestPList");

            npcTemplate->attackToEastAnimationPList = tabFileReader.getString(i, "AttackToEastPList");
            npcTemplate->attackToNorthEastAnimationPList = tabFileReader.getString(i, "AttackToNorthEastPList");
            npcTemplate->attackToSouthEastAnimationPList = tabFileReader.getString(i, "AttackToSouthEastPList");
            npcTemplate->attackToWestAnimationPList = tabFileReader.getString(i, "AttackToWestPList");
            npcTemplate->attackToNorthWestAnimationPList = tabFileReader.getString(i, "AttackToNorthWestPList");
            npcTemplate->attackToSouthWestAnimationPList = tabFileReader.getString(i, "AttackToSouthWestPList");

            npcTemplate->maxHp = tabFileReader.getInteger(i, "MaxHp");
            npcTemplate->attackPower = tabFileReader.getInteger(i, "AttackPower");
            npcTemplate->maxAttackRadius = tabFileReader.getInteger(i, "MaxAttackRadius");
            npcTemplate->maxAlertRadius = tabFileReader.getInteger(i, "MaxAlertRadius");
            npcTemplate->perSecondAttackCount = tabFileReader.getInteger(i, "PerSecondAttackCount");
            npcTemplate->perSecondMoveSpeedByPixel = tabFileReader.getFloat(i, "PerSecondMoveSpeedByPixel");

            auto bulletTypeName = tabFileReader.getString(i, "BulletType");
            auto bulletTypeIter = s_bulletTypeStringToEnum.find(bulletTypeName);
            if (bulletTypeIter != s_bulletTypeStringToEnum.end())
            {
                npcTemplate->bulletType = s_bulletTypeStringToEnum[bulletTypeName];
            }

            auto damageTypeName = tabFileReader.getString(i, "DamageType");
            auto damageTypeIter = s_damageTypeStringToEnum.find(damageTypeName);
            if (damageTypeIter != s_damageTypeStringToEnum.end())
            {
                npcTemplate->damageType = s_damageTypeStringToEnum[damageTypeName];
            }

            npcTemplate->aoeDamageRadius = tabFileReader.getFloat(i, "AOEDamageRadius");

            npcTemplate->shadowYPosition = tabFileReader.getFloat(i, "ShadowYPosition");
            npcTemplate->hpBarYPosition = tabFileReader.getFloat(i, "HpBarYPosition");

            npcTemplate->moveAnimateDelayPerUnit = tabFileReader.getFloat(i, "MoveAnimateDelayPerUnit");
            npcTemplate->standAnimateDelayPerUnit = tabFileReader.getFloat(i, "StandAnimateDelayPerUnit");
            npcTemplate->dieAnimateDelayPerUnit = tabFileReader.getFloat(i, "DieAnimateDelayPerUnit");

            npcTemplate->reinforceRadius = tabFileReader.getFloat(i, "ReinforceRadius");

            npcTemplate->blueSelectedTipsTextureName = tabFileReader.getString(i, "BlueSelectedTipsTextureName");
            npcTemplate->redSelectedTipsTextureName = tabFileReader.getString(i, "RedSelectedTipsTextureName");

            npcTemplate->technologyPointForEnemy = tabFileReader.getInteger(i, "TechnologyPointForEnemy");

            _npcTemplatesMap[templateName] = npcTemplate;
        }

        result = true;
    }

    return result;
}

bool TemplateManager::initBulletTemplates()
{
    bool result = false;

    TabFileReader tabFileReader;
    if (tabFileReader.open("BulletTemplates.tab"))
    {
        for (int i = 0; i < tabFileReader.getRowCount(); i++)
        {
            auto bulletTypeName = tabFileReader.getString(i, "BulletType");

            BulletTemplate* bulletTemplate = new BulletTemplate();
            bulletTemplate->bulletFileName = tabFileReader.getString(i, "BulletFileName");

            auto bulletTypeIter = s_bulletTypeStringToEnum.find(bulletTypeName);
            CCASSERT(bulletTypeIter != s_bulletTypeStringToEnum.end(), StringUtils::format("%s is invalid type", bulletTypeName.c_str()).c_str());
            
            auto bulletType = bulletTypeIter->second;
            _bulletTemplatesMap[bulletType] = bulletTemplate;
        }

        result = true;
    }

    return result;
}

bool TemplateManager::initBuildingTemplates()
{
    bool result = false;

    TabFileReader tabFileReader;
    if (tabFileReader.open("BuildingTemplates.tab"))
    {
        for (int i = 0; i < tabFileReader.getRowCount(); i ++)
        {
            BuildingTemplate* buildingTemplate = new BuildingTemplate();

            buildingTemplate->prepareToBuildStatusTextureName = tabFileReader.getString(i, "PrepareToBuildStatusTextureName");
            buildingTemplate->beingBuiltStatusTextureName = tabFileReader.getString(i, "BeingBuiltStatusTextureName");
            buildingTemplate->workingStatusTextureName = tabFileReader.getString(i, "WorkingStatusTextureName");
            buildingTemplate->destroyStatusTextureName = tabFileReader.getString(i, "DestroyStatusTextureName");
            buildingTemplate->shadowTextureName = tabFileReader.getString(i, "ShadowTextureName");
            buildingTemplate->destroySpecialEffectTemplateName = tabFileReader.getString(i, "DestroySpecialEffectTemplateName");
            buildingTemplate->shadowYPositionInBeingBuiltStatus = tabFileReader.getFloat(i, "ShadowYPositionInBeingBuiltStatus");
            buildingTemplate->shadowYPositionInWorkingStatus = tabFileReader.getFloat(i, "ShadowYPositionInWorkingStatus");
            buildingTemplate->shadowYPositionInDestroyStatus = tabFileReader.getFloat(i, "ShadowYPositionInDestroyStatus");
            buildingTemplate->buildingTimeBySecond = tabFileReader.getFloat(i, "BuildingTimeBySecond");
            buildingTemplate->extraEnemyAttackRadius = tabFileReader.getFloat(i, "ExtraEnemyAttackRadius");
            buildingTemplate->maxHP = tabFileReader.getInteger(i, "MaxHP");
            buildingTemplate->centerBottomGridYPosition = tabFileReader.getFloat(i, "CenterBottomGridYPosition");
            buildingTemplate->bottomGridColumnCount = tabFileReader.getInteger(i, "BottomGridColumnCount");
            buildingTemplate->bottomGridRowCount = tabFileReader.getInteger(i, "BottomGridRowCount");
            buildingTemplate->defenceNpcName = tabFileReader.getString(i, "DefenceNpcName");
            buildingTemplate->defenceNpcYPosition = tabFileReader.getFloat(i, "DefenceNpcYPosition");
            buildingTemplate->attackRange = tabFileReader.getFloat(i, "AttackRange");
            buildingTemplate->attackPower = tabFileReader.getFloat(i, "AttackPower");
            buildingTemplate->blueSelectedTipsTextureName = tabFileReader.getString(i, "BlueSelectedTipsTextureName");
            buildingTemplate->redSelectedTipsTextureName = tabFileReader.getString(i, "RedSelectedTipsTextureName");
            buildingTemplate->technologyPointForEnemy = tabFileReader.getInteger(i, "TechnologyPointForEnemy");

            auto buildingTemplateName = tabFileReader.getString(i, "BuildingTemplateName");
            _buildingTemplatesMap[buildingTemplateName] = buildingTemplate;
        }

        result = true;
    }

    return result;
}

bool TemplateManager::initSpecialEffectTemplates()
{
    bool result = false;

    TabFileReader tabFileReader;
    if (tabFileReader.open("SpecialEffectTemplates.tab"))
    {
        for (int i = 0; i < tabFileReader.getRowCount(); i ++)
        {
            SpecialEffectTemplate* specialEffectTemplate = new SpecialEffectTemplate();

            specialEffectTemplate->animationPListName = tabFileReader.getString(i, "AnimationPListName");
            specialEffectTemplate->perUnitIntervalBySecond = tabFileReader.getFloat(i, "PerUnitIntervalBySecond");

            auto templateName = tabFileReader.getString(i, "TemplateName");
            _specialEffectTemplatesMap[templateName] = specialEffectTemplate;
        }

        result = true;
    }

    return result;
}

const NpcTemplatesMap& TemplateManager::getNpcTemplatesMap()
{
    return _npcTemplatesMap;
}
