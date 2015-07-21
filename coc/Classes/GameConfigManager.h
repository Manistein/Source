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

    string balloonTemplateName;
    int balloonReinforceCount = 0;

    string gargTemplateName;
    int gargReinforceCount = 0;
};

struct GameObjectLevelConfig
{
    int attackPower = 0;
    int hp = 0;
    string levelRepresentTextureName;
    int costTechnologyPoint = 0;
};

struct StageConfig
{
    string mapName;
    float easyModeFactor = 0.0f;
    float normalModeFactor = 0.0f;
    float hardModeFactor = 0.0f;
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
    const StageConfig* getStageConfigBy(int stageID);
    string getStageIntroductionBy(int stageIndex);
private:
    bool init();
    bool initReinforcementConfig();
    bool initGameObjectLevelConfig();
    bool initStageConfig();
    bool initStageIntroduction();

    map<ForceType, ReinforceConfig*> _reinforceConfigMap;
    GameObjectLevelConfigMap _gameObjectLevelConfigMap;

    typedef int StageID;
    map<StageID, StageConfig> _stageConfigMap;
    map<StageID, string> _stageIntrodutionMap;

    GameConfigManager(){};
    GameConfigManager(const GameConfigManager&);
    GameConfigManager& operator = (const GameConfigManager&);
};