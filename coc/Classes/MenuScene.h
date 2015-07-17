#pragma once

class StorageManager;

class MenuScene : public Layer
{
public:
    static cocos2d::Scene* createScene();

    CREATE_FUNC(MenuScene);
private:
    bool init() override;
    void initSettingDialog();
    void initDeveloperDialog();
    void initButtons();

    void onGameStart(Ref* sender, Widget::TouchEventType type);
    void onOpenSettingDialog(Ref* sender, Widget::TouchEventType type);
    void onOpenDevelopersDialog(Ref* sender, Widget::TouchEventType type);
    void onGameExit(Ref* sender, Widget::TouchEventType type);
    void onSettingDialogClose(Ref* sender, Widget::TouchEventType type);
    void onDeveloperDialogClose(Ref* sender, Widget::TouchEventType type);

    void onMusicVolumeSliderMoved(Ref* sender, Slider::EventType sliderType);
    void onEffectVolumeSliderMoved(Ref* sender, Slider::EventType sliderType);

    Node* _mainMenu = nullptr;
    Node* _mainMenuPanel = nullptr;

    Layout* _settingDialog = nullptr;
    Layout* _developerDialog = nullptr;

    StorageManager* _storageManager = nullptr;
};