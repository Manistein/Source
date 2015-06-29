#pragma once

struct NpcSoundEffectData
{
    string attackName;
    string deathName;
    string selectName;
    string moveName;
};

struct BuildingSoundEffectData
{
    string destroyedName;
    string constructName;
};

enum class NpcSoundEffectType
{
    Attack,
    Death,
    Select,
    Move
};

enum class BuildingSoundEffectType
{
    Construct,
    Destroyed
};

enum class UIEffectType
{
    ReinforcePointIncrease,
    TrainFinished,
    ButtonClick
};

class SoundManager
{
public:
    ~SoundManager();

    static SoundManager* getInstance();

    void setVolume(float volume);
    float getVolume();

    void playRandomBackgroundMusicOneByOne();
    void playNpcEffect(const string& templateName, NpcSoundEffectType type);
    void playBuildingEffect(BuildingSoundEffectType type);
    void playUIEffect(UIEffectType type);
private:
    bool init();
    bool initBackgroundMusicData();
    bool initNpcSoundEffectData();
    bool initBuildingSoundEffectData();
    bool initUISoundEffectData();

    vector<string> _backgroundMusicNameList;
    BuildingSoundEffectData _buildingSoundEffectData;

    typedef string TemplateName;
    unordered_map<TemplateName, NpcSoundEffectData*> _npcSoundEffectDataMap;

    typedef string TypeName;
    unordered_map<TypeName, string> _uiSoundEffectNameMap;

    float _volume = 1.0f; // “Ù¡ø¥Û–°£¨0.0f~1.0f

    SoundManager(){};
    SoundManager(const SoundManager&);
    SoundManager& operator = (const SoundManager&);
};