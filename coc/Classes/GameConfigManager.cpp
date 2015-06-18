#include "Base.h"
#include "GameObject.h"
#include "GameConfigManager.h"
#include "TabFileReader.h"

static GameConfigManager* s_gameConfigManager = nullptr;

GameConfigManager::~GameConfigManager()
{
    for (auto& iter : _reinforceConfigMap)
    {
        CC_SAFE_DELETE(iter.second);
    }
    _reinforceConfigMap.clear();
}


GameConfigManager* GameConfigManager::getInstance()
{
    if (s_gameConfigManager == nullptr)
    {
        s_gameConfigManager = new GameConfigManager();
        s_gameConfigManager->init();
    }

    return s_gameConfigManager;
}

bool GameConfigManager::init()
{
    bool reslut = false;

    reslut = initReinforcementConfig();

    return reslut;
}

bool GameConfigManager::initReinforcementConfig()
{
    TabFileReader tabFileReader;
    if (tabFileReader.open("ReinforcementConfig.tab"))
    {
        for (int i = 0; i < tabFileReader.getRowCount(); i++)
        {
            auto reinforceConfig = new ReinforceConfig();

            reinforceConfig->enchanterTemplateName = tabFileReader.getString(i, "EnchanterTemplateName");
            reinforceConfig->enchanterReinforceCount = tabFileReader.getInteger(i, "EnchanterReinforceCount");
            reinforceConfig->archerTemplateName = tabFileReader.getString(i, "ArcherTemplateName");
            reinforceConfig->archerReinforceCount = tabFileReader.getInteger(i, "ArcherReinforceCount");
            reinforceConfig->barbarianTemplateName = tabFileReader.getString(i, "BarbarianTemplateName");
            reinforceConfig->barbarianReinforceCount = tabFileReader.getInteger(i, "BarbarianReinforceCount");
            reinforceConfig->enchanterTowerTemplateName = tabFileReader.getString(i, "EnchanterTowerTemplateName");
            reinforceConfig->archerTowerTemplateName = tabFileReader.getString(i, "ArcherTowerTempalteName");

            string forceTypeString = tabFileReader.getString(i, "ForceType");
            if (forceTypeString == "Player")
            {
                _reinforceConfigMap[ForceType::Player] = reinforceConfig;
            }
            else
            {
                _reinforceConfigMap[ForceType::AI] = reinforceConfig;
            }
        }
    }

    return true;
}

const ReinforceConfig* GameConfigManager::getReinforceConfigBy(ForceType forceType)
{
    if (forceType == ForceType::Invalid)
    {
        return nullptr;
    }

    return _reinforceConfigMap[forceType];
}
