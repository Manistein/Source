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

    for (auto& iter : _gameObjectLevelConfigMap)
    {
        for (auto& subIter : iter.second)
        {
            CC_SAFE_DELETE(subIter.second);
        }
        iter.second.clear();
    }
    _gameObjectLevelConfigMap.clear();
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
    reslut = initGameObjectLevelConfig();
    reslut = initStageConfig();
    reslut = initStageIntroduction();

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
            reinforceConfig->balloonTemplateName = tabFileReader.getString(i, "BalloonTemplateName");
            reinforceConfig->balloonReinforceCount = tabFileReader.getInteger(i, "BalloonReinforceCount");
            reinforceConfig->gargTemplateName = tabFileReader.getString(i, "GargTemplateName");
            reinforceConfig->gargReinforceCount = tabFileReader.getInteger(i, "GargReinforceCount");

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

bool GameConfigManager::initStageConfig()
{
    bool result = true;

    TabFileReader tabFileReader;
    if (tabFileReader.open("StageConfig.tab"))
    {
        for (int i = 0; i < tabFileReader.getRowCount(); i++)
        {
            StageConfig stageConfig;
            stageConfig.mapName = tabFileReader.getString(i, "MapName");
            stageConfig.easyModeFactor = tabFileReader.getFloat(i, "EasyFactor");
            stageConfig.normalModeFactor = tabFileReader.getFloat(i, "NormalFactor");
            stageConfig.hardModeFactor = tabFileReader.getFloat(i, "HardFactor");

            int stageID = tabFileReader.getInteger(i, "StageID");
            _stageConfigMap[stageID] = stageConfig;
        }
    }


    return result;
}

bool GameConfigManager::initStageIntroduction()
{
    TabFileReader tabFileReader;
    if (tabFileReader.open("StageIntroduction.tab"))
    {
        for (int i = 0; i < tabFileReader.getRowCount(); i ++)
        {
            int stageIndex = i + 1;
            _stageIntrodutionMap[stageIndex] = tabFileReader.getString(i, "Introduction");
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

bool GameConfigManager::initGameObjectLevelConfig()
{
    TabFileReader tabFileReader;
    if (tabFileReader.open("LevelConfig.tab"))
    {
        for (int rowIndex = 0; rowIndex < tabFileReader.getRowCount(); rowIndex ++)
        {
            auto templateName = tabFileReader.getString(rowIndex, "TemplateName");
            _gameObjectLevelConfigMap[templateName] = LevelConfigMap();

            for (int level = 1; level <= 5; level ++)
            {
                auto levelConfig = new GameObjectLevelConfig();

                auto attackPowerColumnName = StringUtils::format("Level%dAttackPower", level);
                levelConfig->attackPower = tabFileReader.getInteger(rowIndex, attackPowerColumnName.c_str());

                auto hpColumnName = StringUtils::format("Level%dHP", level);
                levelConfig->hp = tabFileReader.getInteger(rowIndex, hpColumnName.c_str());

                auto levelRepresentTextureNameColumnName = StringUtils::format("Level%dRepresentTextureName", level);
                levelConfig->levelRepresentTextureName = tabFileReader.getString(rowIndex, levelRepresentTextureNameColumnName.c_str());

                auto costTechnologyPointColumnName = StringUtils::format("Level%dCostTechnologyPoint", level);
                levelConfig->costTechnologyPoint = tabFileReader.getInteger(rowIndex, costTechnologyPointColumnName.c_str());

                _gameObjectLevelConfigMap[templateName][level] = levelConfig;
            }
        }
    }

    return true;
}

const GameObjectLevelConfig* GameConfigManager::getGameObjectLevelConfig(const string& templateName, int level)
{
    GameObjectLevelConfig* gameObjectLevelConfig = nullptr;

    auto levelConfigMapIter = _gameObjectLevelConfigMap.find(templateName);
    if (levelConfigMapIter != _gameObjectLevelConfigMap.end())
    {
        auto gameObjectLevelConfigIter = levelConfigMapIter->second.find(level);
        if (gameObjectLevelConfigIter != levelConfigMapIter->second.end())
        {
            gameObjectLevelConfig = gameObjectLevelConfigIter->second;
        }
    }

    return gameObjectLevelConfig;
}

const StageConfig* GameConfigManager::getStageConfigBy(int stageID)
{
    StageConfig* stageConfig = nullptr;

    auto stageConfigIter = _stageConfigMap.find(stageID);
    if (stageConfigIter != _stageConfigMap.end())
    {
        stageConfig = &stageConfigIter->second;
    }

    return stageConfig;
}

std::string GameConfigManager::getStageIntroductionBy(int stageIndex)
{
    string introduction;

    auto infoIter = _stageIntrodutionMap.find(stageIndex);
    if (infoIter != _stageIntrodutionMap.end())
    {
        introduction = infoIter->second;
    }

    return introduction;
}
