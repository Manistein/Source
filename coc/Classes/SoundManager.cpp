#include "Base.h"
#include "audio/include/AudioEngine.h"
#include "SoundManager.h"
#include "TabFileReader.h"
#include  <io.h>
#include  <stdio.h>
#include  <stdlib.h>

using namespace experimental;

static SoundManager* s_soundManager = nullptr;

SoundManager::~SoundManager()
{
    for (auto& iter : _npcSoundEffectDataMap)
    {
        CC_SAFE_DELETE(iter.second);
    }
    _npcSoundEffectDataMap.clear();
}

SoundManager* SoundManager::getInstance()
{
    if (s_soundManager == nullptr)
    {
        s_soundManager = new SoundManager();
        s_soundManager->init();
    }

    return s_soundManager;
}

bool SoundManager::init()
{
    AudioEngine::lazyInit();

    initBackgroundMusicData();
    initNpcSoundEffectData();
    initBuildingSoundEffectData();
    initUISoundEffectData();

    srand(::timeGetTime());

    return true;
}

bool SoundManager::initBackgroundMusicData()
{
    TabFileReader tabFileReader;
    if (tabFileReader.open("BackgroundMusic.tab"))
    {
        for (int i = 0; i < tabFileReader.getRowCount(); i++)
        {
            auto backgroundMusicName = tabFileReader.getString(i, "BackgroundMusicName");
            _backgroundMusicNameList.push_back(backgroundMusicName);
        }
    }

    return true;
}

bool SoundManager::initNpcSoundEffectData()
{
    TabFileReader tabFileReader;
    if (tabFileReader.open("NpcSoundEffect.tab"))
    {
        for (int i = 0; i < tabFileReader.getRowCount(); i++)
        {
            auto npcSoundEffectData = new NpcSoundEffectData();
            npcSoundEffectData->attackName = tabFileReader.getString(i, "AttackEffectName");
            npcSoundEffectData->deathName = tabFileReader.getString(i, "DeathEffectName");
            npcSoundEffectData->selectName = tabFileReader.getString(i, "Select");
            npcSoundEffectData->moveName = tabFileReader.getString(i, "Move");

            auto templateName = tabFileReader.getString(i, "TemplateName");
            _npcSoundEffectDataMap[templateName] = npcSoundEffectData;
        }
    }

    return true;
}

bool SoundManager::initBuildingSoundEffectData()
{
    TabFileReader tabFileReader;
    if (tabFileReader.open("BuildingSoundEffect.tab"))
    {
        _buildingSoundEffectData.constructName = tabFileReader.getString(0, "ConstructEffectName");
        _buildingSoundEffectData.destroyedName = tabFileReader.getString(0, "DestroyEffectName");
    }

    return true;
}

void SoundManager::setEffectVolume(float volume)
{
    _effectVolume = std::max(0.0f, volume);
    _effectVolume = std::min(1.0f, volume);
}

float SoundManager::getEffectVolume()
{
    return _effectVolume;
}

void SoundManager::setMusicVolume(float volume)
{
    _musicVolume = std::max(0.0f, volume);
    _musicVolume = std::min(1.0f, volume);

    if (_musicAudioID != AudioEngine::INVALID_AUDIO_ID)
    {
        AudioEngine::setVolume(_musicAudioID, volume);
    }
}

float SoundManager::getMusicVolume()
{
    return _musicVolume;
}

void SoundManager::playMusic(const string& musicName, bool isLoop)
{
    if (canPlay(musicName))
    {
        _musicAudioID = AudioEngine::play2d(musicName, isLoop, _musicVolume);
    }
}

void SoundManager::playRandomBackgroundMusicOneByOne()
{
    if (_backgroundMusicNameList.empty())
    {
        return;
    }

    int randomIndex = rand() % (int)_backgroundMusicNameList.size();
    auto backgroundMusicName = _backgroundMusicNameList[randomIndex];

    if (canPlay(backgroundMusicName))
    {
        int audioID = AudioEngine::play2d(backgroundMusicName, false, _musicVolume);
        AudioEngine::setFinishCallback(audioID, CC_CALLBACK_0(SoundManager::playRandomBackgroundMusicOneByOne, this));

        _musicAudioID = audioID;
    }
}

void SoundManager::playNpcEffect(const string& templateName, NpcSoundEffectType type)
{
    auto npcSoundEffectIter = _npcSoundEffectDataMap.find(templateName);
    if (npcSoundEffectIter != _npcSoundEffectDataMap.end())
    {
        auto npcSoundEffectData = npcSoundEffectIter->second;
        string soundEffectName;

        switch (type)
        {
        case NpcSoundEffectType::Attack:
            soundEffectName = npcSoundEffectData->attackName;
            break;
        case NpcSoundEffectType::Death:
            soundEffectName = npcSoundEffectData->deathName;
            break;
        case NpcSoundEffectType::Select:
            soundEffectName = npcSoundEffectData->selectName;
            break;
        case NpcSoundEffectType::Move:
            soundEffectName = npcSoundEffectData->moveName;
            break;
        default:    break;
        }

        if (canPlay(soundEffectName))
        {
            AudioEngine::play2d(soundEffectName, false, _effectVolume);
        }
    }
}

void SoundManager::playBuildingEffect(BuildingSoundEffectType type)
{
    if (type == BuildingSoundEffectType::Construct)
    {
        if (canPlay(_buildingSoundEffectData.constructName))
        {
            AudioEngine::play2d(_buildingSoundEffectData.constructName, false, _effectVolume);
        }
    }
    else
    {
        if (canPlay(_buildingSoundEffectData.destroyedName))
        {
            AudioEngine::play2d(_buildingSoundEffectData.destroyedName, false, _effectVolume);
        }
    }
}

void SoundManager::playUIEffect(UIEffectType type)
{
    string effectName;
    switch (type)
    {
    case UIEffectType::ReinforcePointIncrease:
        effectName = _uiSoundEffectNameMap["ReinforcePointIncrease"];
        break;
    case UIEffectType::TrainFinished:
        effectName = _uiSoundEffectNameMap["TrainFinished"];
        break;
    case UIEffectType::ButtonClick:
        effectName = _uiSoundEffectNameMap["ButtonClick"];
        break;
    default:
        break;
    }

    if (canPlay(effectName))
    {
        AudioEngine::play2d(effectName, false, _effectVolume);
    }
}

bool SoundManager::initUISoundEffectData()
{
    TabFileReader tabFileReader;
    if (tabFileReader.open("UISoundEffect.tab"))
    {
        for (int i = 0; i < tabFileReader.getRowCount(); i ++)
        {
            auto effectName = tabFileReader.getString(i, "EffectName");
            auto typeName = tabFileReader.getString(i, "TypeName");

            _uiSoundEffectNameMap[typeName] = effectName;
        }
    }

    return true;
}

void SoundManager::stopAll()
{
    AudioEngine::stopAll();

    _musicAudioID = AudioEngine::INVALID_AUDIO_ID;
}

void SoundManager::checkBackgroundMusicStatus()
{
    static float lastTimeBySecond = ::timeGetTime() / 1000.0f;
    static float currentTimeBySecond = lastTimeBySecond;

    currentTimeBySecond = ::timeGetTime() / 1000.0f;
    if (currentTimeBySecond - lastTimeBySecond > 30.0f)
    {
        auto audioStatus = AudioEngine::getState(_musicAudioID);
        if (audioStatus == AudioEngine::AudioState::PAUSED)
        {
            AudioEngine::resume(_musicAudioID);
        }
        else if (audioStatus == AudioEngine::AudioState::ERROR)
        {
            playRandomBackgroundMusicOneByOne();
        }

        lastTimeBySecond = currentTimeBySecond;
    }
}

bool SoundManager::canPlay(const string& fileName)
{
    bool result = false;

    auto fullPath = FileUtils::getInstance()->fullPathForFilename(fileName);
    if (_access(fullPath.c_str(), 0) != -1)
    {
        result = true;
    }

    return result;
}
