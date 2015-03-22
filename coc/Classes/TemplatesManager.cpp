#include "Base.h"
#include "GameObject.h"
#include "TemplatesManager.h"
#include "TabFileReader.h"
#include <unordered_map>

static TemplateManager* s_templateManager = nullptr;

static unordered_map<string, BulletType> s_bulletTypeStringToEnum = {
    { "Invalid", BulletType::Invalid },
    { "Arrow", BulletType::Arrow },
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

bool TemplateManager::init()
{
    bool result = false;

    result = initNpcTemplates();
    result = initBulletTemplates();

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
            auto jobName = tabFileReader.getString(i, "NpcJobName");

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

            _npcTemplatesMap[jobName] = npcTemplate;
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
            bulletTemplate->bulletPList = tabFileReader.getString(i, "BulletPList");

            auto bulletTypeIter = s_bulletTypeStringToEnum.find(bulletTypeName);
            CCASSERT(bulletTypeIter != s_bulletTypeStringToEnum.end(), StringUtils::format("%s is invalid type", bulletTypeName.c_str()).c_str());
            
            auto bulletType = bulletTypeIter->second;
            _bulletTemplatesMap[bulletType] = bulletTemplate;
        }

        result = true;
    }

    return result;
}
