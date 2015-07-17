#pragma once

class cocos2d::UserDefault;
class SoundManager;

struct StageData 
{
    int maxPlayableStage = FIRST_STAGE;
    int playerSelectedStage = FIRST_STAGE;
};

class StorageManager
{
public:
    ~StorageManager();

    static StorageManager* getInstance();
    void load();
    void save();

    StageData _stageData;
private:
    void init();

    UserDefault* _userDefault = nullptr;
    SoundManager* _soundManager = nullptr;

    StorageManager(){}
    StorageManager(const StorageManager&);
    StorageManager& operator = (const StorageManager&);
};