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

class GameConfigManager
{
public:
    ~GameConfigManager();

    static GameConfigManager* getInstance();
    const ReinforceConfig* getReinforceConfigBy(ForceType forceType);
private:
    bool init();
    bool initReinforcementConfig();

    map<ForceType, ReinforceConfig*> _reinforceConfigMap;

    GameConfigManager(){};
    GameConfigManager(const GameConfigManager&);
    GameConfigManager& operator = (const GameConfigManager&);
};