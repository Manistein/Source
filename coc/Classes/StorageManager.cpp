#include "Base.h"
#include "StorageManager.h"
#include "base/CCUserDefault.h"
#include "audio/include/AudioEngine.h"
#include "SoundManager.h"

static StorageManager* s_storageManager = nullptr;

StorageManager::~StorageManager()
{

}

StorageManager* StorageManager::getInstance()
{
    if (s_storageManager == nullptr)
    {
        s_storageManager = new StorageManager();
        s_storageManager->init();
    }

    return s_storageManager;
}

void StorageManager::init()
{
    _userDefault = cocos2d::UserDefault::getInstance();
    _soundManager = SoundManager::getInstance();
}

void StorageManager::load()
{
    float musicVolume = _userDefault->getFloatForKey("MusicVolumePercent", MAX_VOLUME);
    float soundEffectVolume = _userDefault->getFloatForKey("SoundEffectVolumePercent", MAX_VOLUME);
    _soundManager->setMusicVolume(musicVolume);
    _soundManager->setEffectVolume(soundEffectVolume);

    _stageData.maxPlayableStage = _userDefault->getIntegerForKey("MaxPlayableStage", FIRST_STAGE);
    _stageData.playerSelectedStage = _userDefault->getIntegerForKey("SelectedStage", FIRST_STAGE);

    log(_userDefault->getXMLFilePath().c_str());
}

void StorageManager::save()
{
    _userDefault->setFloatForKey("MusicVolumePercent", _soundManager->getMusicVolume());
    _userDefault->setFloatForKey("SoundEffectVolumePercent", _soundManager->getEffectVolume());
    _userDefault->setIntegerForKey("MaxPlayableStage", _stageData.maxPlayableStage);
    _userDefault->setIntegerForKey("SelectedStage", _stageData.playerSelectedStage);
}
