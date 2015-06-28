#pragma once

struct NpcSoundEffectData
{
    string attackName;
    string deathName;
};

struct BuildingSoundEffectData
{
    string destroyedName;
    string constructName;
};

enum class NpcSoundEffectType
{
    Attack,
    Death
};

enum class BuildingSoundEffectType
{
    Construct,
    Destroyed
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
private:
    bool init();
    bool initBackgroundMusicData();
    bool initNpcSoundEffectData();
    bool initBuildingSoundEffectData();

    vector<string> _backgroundMusicNameList;
    BuildingSoundEffectData _buildingSoundEffectData;

    typedef string TemplateName;
    unordered_map<TemplateName, NpcSoundEffectData*> _npcSoundEffectDataMap;

    float _volume = 1.0f; // “Ù¡ø¥Û–°£¨0.0f~1.0f

    SoundManager(){};
    SoundManager(const SoundManager&);
    SoundManager& operator = (const SoundManager&);
};