#include "Base.h"
#include "GameObject.h"
#include "LoadingScene.h"
#include "TemplatesManager.h"
#include "Utils.h"
#include "GameScene.h"
#include "GameSetting.h"

cocos2d::Scene* LoadingScene::createScene()
{
    auto scene = Scene::create();

    auto layer = LoadingScene::create();
    scene->addChild(layer);

    return scene;
}

bool LoadingScene::init()
{
    if (!Layer::init())
    {
        return false;
    }

    auto winSize = Director::getInstance()->getWinSize();

    TTFConfig ttfConfig("arial.ttf", 48);
    _loadingLabel = Label::createWithTTF(ttfConfig, "0%");
    _loadingLabel->setPosition(winSize.width / 2.0f, winSize.height / 2.0f);
    addChild(_loadingLabel);

    initGameObjectResources();

    Director::getInstance()->resume();
    scheduleUpdate();

    return true;
}

void LoadingScene::initGameObjectResources()
{
    _plistFileNameList.push_back("BuildingCommon.plist");
    _plistFileNameList.push_back("LevelRanks.plist");

    auto& npcTemplateMap = TemplateManager::getInstance()->getNpcTemplatesMap();
    for (auto& iter : npcTemplateMap)
    {
        _plistFileNameList.push_back(iter.second->attackToEastAnimationPList);
        _plistFileNameList.push_back(iter.second->attackToNorthEastAnimationPList);
        _plistFileNameList.push_back(iter.second->attackToNorthWestAnimationPList);
        _plistFileNameList.push_back(iter.second->attackToSouthEastAnimationPList);
        _plistFileNameList.push_back(iter.second->attackToSouthWestAnimationPList);
        _plistFileNameList.push_back(iter.second->attackToWestAnimationPList);

        _plistFileNameList.push_back(iter.second->moveToEastAnimationPList);
        _plistFileNameList.push_back(iter.second->moveToNorthEastAnimationPList);
        _plistFileNameList.push_back(iter.second->moveToNorthWestAnimationPList);
        _plistFileNameList.push_back(iter.second->moveToSouthEastAnimationPList);
        _plistFileNameList.push_back(iter.second->moveToSouthWestAnimationPList);
        _plistFileNameList.push_back(iter.second->moveToWestAnimationPList);

        _plistFileNameList.push_back(iter.second->standAndFaceToEastAnimationPList);
        _plistFileNameList.push_back(iter.second->standAndFaceToNorthEastAnimationPList);
        _plistFileNameList.push_back(iter.second->standAndFaceToNorthWestAnimationPList);
        _plistFileNameList.push_back(iter.second->standAndFaceToSouthEastAnimationPList);
        _plistFileNameList.push_back(iter.second->standAndFaceToSouthWestAnimationPList);
        _plistFileNameList.push_back(iter.second->standAndFaceToWestAnimationPList);

        _plistFileNameList.push_back(iter.second->dieAnimationPList);
    }
}

void LoadingScene::update(float delta)
{
    if (_loadingIndex < (int)_plistFileNameList.size())
    {
        if (!g_setting.hasLoadGameResouce)
        {
            SpriteFrameCache::getInstance()->addSpriteFramesWithFile(_plistFileNameList[_loadingIndex]);
            GameUtils::createAnimationWithPList(_plistFileNameList[_loadingIndex]);
        }

        _loadingIndex++;
        int totalNum = (int)_plistFileNameList.size();
        float percentage = (float)_loadingIndex / (float)totalNum * 100.0f;
        _loadingLabel->setString(StringUtils::format("Loading... %d", (int)percentage) + "%");
    }
    else
    {
        g_setting.hasLoadGameResouce = true;
        auto gameScene = GameScene::createScene();
        Director::getInstance()->replaceScene(gameScene);
    }
}
