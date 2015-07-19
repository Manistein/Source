#pragma once

struct ReinforceConfig
{
    string enchanterTemplateName;
    int enchanterReinforceCount = 0;

    string archerTemplateName;
    int archerReinforceCount = 0;

    string barbarianTemplateName;
    int barbarianReinforceCount = 0;

    string enchanterTowerTemplateName;
    string archerTowerTemplateName;
};

struct GameObjectLevelConfig
{
    int attackPower = 0;
    int hp = 0;
    string levelRepresentTextureName;
    int costTechnologyPoint = 0;
};

typedef int Level;
typedef unordered_map<Level, GameObjectLevelConfig*> LevelConfigMap;
typedef unordered_map<string, LevelConfigMap> GameObjectLevelConfigMap;

class GameConfigManager
{
public:
    ~GameConfigManager();

    static GameConfigManager* getInstance();
    const ReinforceConfig* getReinforceConfigBy(ForceType forceType);
    const GameObjectLevelConfig* getGameObjectLevelConfig(const string& templateName, int level);
    string getStageIntroductionBy(int stageIndex);
private:
    bool init();
    bool initReinforcementConfig();
    bool initGameObjectLevelConfig();
    bool initStageIntroduction();

    map<ForceType, ReinforceConfig*> _reinforceConfigMap;
    GameObjectLevelConfigMap _gameObjectLevelConfigMap;

    map<int, string> _stageIntrodutionMap;

    GameConfigManager(){};
    GameConfigManager(const GameConfigManager&);
    GameConfigManager& operator = (const GameConfigManager&);
};