#include "Base.h"
#include "MenuScene.h"
#include "cocostudio/ActionTimeline/CSLoader.h"
#include "GameScene.h"
#include "audio/include/AudioEngine.h"
#include "SoundManager.h"
#include "LoadingScene.h"

cocos2d::Scene* MenuScene::createScene()
{
    auto scene = Scene::create();

    auto layer = MenuScene::create();
    scene->addChild(layer);

    return scene;
}

bool MenuScene::init()
{
    if (!Layer::init())
    {
        return false;
    }

    _mainMenu = CSLoader::createNode("MenuScene.csb");
    addChild(_mainMenu);

    _mainMenuPanel = _mainMenu->getChildByName("Panel_MainMenu");

    initSettingDialog();
    initDeveloperDialog();
    initButtons();

    SoundManager::getInstance()->playMusic("Noddinagushpa.mp3", true);

    Director::getInstance()->setProjection(Director::Projection::_2D);
    Director::getInstance()->setDepthTest(true);

    return true;
}

void MenuScene::initSettingDialog()
{
    _settingDialog = _mainMenuPanel->getChildByName<Layout*>("Panel_SettingDialog");
    auto closeDialogButton = _settingDialog->getChildByName<Button*>("Button_CloseDialog");
    closeDialogButton->addTouchEventListener(CC_CALLBACK_2(MenuScene::onSettingDialogClose, this));

    auto musicVolumeSlider = _settingDialog->getChildByName<Slider*>("Slider_MusicVolume");
    musicVolumeSlider->addEventListener(CC_CALLBACK_2(MenuScene::onMusicVolumeSliderMoved, this));

    auto effectVolumeSlider = _settingDialog->getChildByName<Slider*>("Slider_EffectVolume");
    effectVolumeSlider->addEventListener(CC_CALLBACK_2(MenuScene::onEffectVolumeSliderMoved, this));

    _settingDialog->setVisible(false);
}

void MenuScene::initDeveloperDialog()
{
    _developerDialog = _mainMenuPanel->getChildByName<Layout*>("Panel_DeveloperIntroduce");
    auto closeDialogButton = _developerDialog->getChildByName<Button*>("Button_DialogClose");
    closeDialogButton->addTouchEventListener(CC_CALLBACK_2(MenuScene::onDeveloperDialogClose, this));

    _developerDialog->setVisible(false);
}

void MenuScene::initButtons()
{
    auto gameStartButton = _mainMenuPanel->getChildByName<Button*>("Button_GameStart");
    gameStartButton->addTouchEventListener(CC_CALLBACK_2(MenuScene::onGameStart, this));

    auto openSettingDialogButton = _mainMenuPanel->getChildByName<Button*>("Button_GameSetting");
    openSettingDialogButton->addTouchEventListener(CC_CALLBACK_2(MenuScene::onOpenSettingDialog, this));

    auto openDeveloperIntroduceDialogButton = _mainMenuPanel->getChildByName<Button*>("Button_GameDeveloper");
    openDeveloperIntroduceDialogButton->addTouchEventListener(CC_CALLBACK_2(MenuScene::onOpenDevelopersDialog, this));

    auto gameExitButton = _mainMenuPanel->getChildByName<Button*>("Button_GameExit");
    gameExitButton->addTouchEventListener(CC_CALLBACK_2(MenuScene::onGameExit, this));
}

void MenuScene::onGameStart(Ref* sender, Widget::TouchEventType type)
{
    if (type == Widget::TouchEventType::ENDED)
    {
        SoundManager::getInstance()->stopAll();

        auto loadingScene = LoadingScene::createScene();
        Director::getInstance()->replaceScene(loadingScene);
    }
}

void MenuScene::onOpenSettingDialog(Ref* sender, Widget::TouchEventType type)
{
    if (type == Widget::TouchEventType::ENDED)
    {
        SoundManager::getInstance()->playUIEffect(UIEffectType::ButtonClick);

        _settingDialog->setVisible(true);
    }
}

void MenuScene::onOpenDevelopersDialog(Ref* sender, Widget::TouchEventType type)
{
    if (type == Widget::TouchEventType::ENDED)
    {
        SoundManager::getInstance()->playUIEffect(UIEffectType::ButtonClick);

        _developerDialog->setVisible(true);
    }
}

void MenuScene::onGameExit(Ref* sender, Widget::TouchEventType type)
{
    if (type == Widget::TouchEventType::ENDED)
    {
        SoundManager::getInstance()->stopAll();

        Director::getInstance()->end();
    }
}

void MenuScene::onSettingDialogClose(Ref* sender, Widget::TouchEventType type)
{
    if (type == Widget::TouchEventType::ENDED)
    {
        SoundManager::getInstance()->playUIEffect(UIEffectType::ButtonClick);

        _settingDialog->setVisible(false);
    }
}

void MenuScene::onDeveloperDialogClose(Ref* sender, Widget::TouchEventType type)
{
    if (type == Widget::TouchEventType::ENDED)
    {
        SoundManager::getInstance()->playUIEffect(UIEffectType::ButtonClick);

        _developerDialog->setVisible(false);
    }
}

void MenuScene::onMusicVolumeSliderMoved(Ref* sender, Slider::EventType sliderType)
{
    if (sliderType == Slider::EventType::ON_PERCENTAGE_CHANGED)
    {
        auto slider = static_cast<Slider*>(sender);
        auto volume = slider->getPercent() / 100.0f;
        SoundManager::getInstance()->setMusicVolume(volume);
    }
}

void MenuScene::onEffectVolumeSliderMoved(Ref* sender, Slider::EventType sliderType)
{
    if (sliderType == Slider::EventType::ON_PERCENTAGE_CHANGED)
    {
        auto slider = static_cast<Slider*>(sender);
        auto volume = slider->getPercent() / 100.0f;
        SoundManager::getInstance()->setEffectVolume(volume);
    }
}
