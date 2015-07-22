#include "Base.h"
#include "MapManager.h"
#include "GameObject.h"
#include "GameUI.h"
#include "DebugInfoLayer.h"
#include "GameWorld.h"
#include "GameWorldCallBackFunctionsManager.h"
#include "cocostudio/ActionTimeline/CSLoader.h"
#include "ForceManager.h"
#include "GameObjectManager.h"
#include "GameConfigManager.h"
#include "GameUICallBackFunctionsManager.h"
#include "audio/include/AudioEngine.h"
#include "SoundManager.h"
#include "GameSetting.h"
#include "Utils.h"
#include "SelectStageScene.h"
#include "MenuScene.h"
#include "WindowsHelper.h"

const string UPGRADE_PROGRESS_BAR_BG = "upgradeProgressBarBG";
const string UPGRADE_PROGRESS_BAR = "upgradeProgressBar";
const string UPGRADE_NEXT_RANK = "nextRank";
const string REINFORCEMENT_RANK = "reinforcementRank";

GameUI::~GameUI()
{

}

bool GameUI::init()
{
    if (!Node::init())
    {
        return false;
    }

    setPositionZ(MAX_GAME_OBJECT_COUNT);

    _debugInfoLayer = DebugInfoLayer::create();
    _debugInfoLayer->setVisible(false);
    addChild(_debugInfoLayer);

    _gameMainUI = CSLoader::createNode("MainScene.csb");
    addChild(_gameMainUI);

    _gameWorld = GameWorldCallBackFunctionsManager::getInstance();
    _forceManager = ForceManager::getInstance();

    initReinforcePresent();
    initMiniMapPresent();
    initReinforcementSelectPanel();
    initUpgradePanel();
    initGameEndTips();
    initOptionPanel();

    Director::getInstance()->getEventDispatcher()->addCustomEventListener("MouseMove", CC_CALLBACK_1(GameUI::onMouseMove, this));

    GameUICallbackFunctionsManager::getInstance()->registerCallBackFunctions(this);

    scheduleUpdate();

    return true;
}

void GameUI::initReinforcePresent()
{
    auto gameMainPanel = _gameMainUI->getChildByName("Panel_GameMain");
    auto reinforceForeground = gameMainPanel->getChildByName("Image_ReinforceCoolDownTimeForeground");
    _askReinforceButton = gameMainPanel->getChildByName<Button*>("Button_AskReinforce");
    _askReinforceButton->setLocalZOrder(1000);

    auto reinforceLogo = Sprite::create("ReinforceCDTimeBG.png");
    auto reinforceProgressBar = ProgressTimer::create(reinforceLogo);
    reinforceProgressBar->setPosition(reinforceForeground->getPosition());

    auto sequenceAction = Sequence::create(
        ProgressTo::create(PLAYER_REINFORCE_TIME_INTERVAL, 100.0f),
        CallFunc::create(CC_CALLBACK_0(ForceManager::onPlayerReinforcePointIncrease, _forceManager)),
        CallFunc::create(CC_CALLBACK_0(ProgressTimer::setPercentage, reinforceProgressBar, 0.0f)),
        CallFunc::create(CC_CALLBACK_0(SoundManager::playUIEffect, SoundManager::getInstance(), UIEffectType::ReinforcePointIncrease)),
        CallFunc::create(CC_CALLBACK_0(GameUI::onUpdateReinforcePresent, this)),
        CallFunc::create(CC_CALLBACK_0(GameUI::onReinforceButtonSparkMove, this)),
        nullptr);
    auto repeatforverAction = RepeatForever::create(sequenceAction);
    reinforceProgressBar->runAction(repeatforverAction);
    
    gameMainPanel->addChild(reinforceProgressBar);


    _reinforceButtonSpark = Sprite::create("Spark.png");
    auto askReinforceButtonSize = _askReinforceButton->getContentSize();
    auto reinforceButtonSparkSize = _reinforceButtonSpark->getContentSize();
    _reinforceButtonSpark->setPosition(-(askReinforceButtonSize.width + reinforceButtonSparkSize.width), 0.0f);
    auto reinforceButtonStencil = Sprite::create("ReinforceButtonNormal.png");

    auto sparkClipNode = ClippingNode::create();
    sparkClipNode->setStencil(reinforceButtonStencil);
    sparkClipNode->setAlphaThreshold(0.01f);
    sparkClipNode->setPosition(_askReinforceButton->getPosition());
    sparkClipNode->setLocalZOrder(_askReinforceButton->getLocalZOrder() + 1);
    sparkClipNode->addChild(_reinforceButtonSpark);

    gameMainPanel->addChild(sparkClipNode);
}

void GameUI::initMiniMapPresent()
{
    auto gameMainPanel = _gameMainUI->getChildByName("Panel_GameMain");
    _minimapImage = gameMainPanel->getChildByName<ImageView*>("Image_MiniMap");

    _minimapDrawNode = DrawNode::create();
    _minimapImage->addChild(_minimapDrawNode, 1);

    _minimapImage->addTouchEventListener(CC_CALLBACK_2(GameUI::onMinimapTouched, this));

    _mapManager = _gameWorld->_getMapManager();

    _minimapWidth = _minimapImage->getContentSize().width;
    _minimapHeight = _minimapImage->getContentSize().height;

    auto mapSize = _mapManager->getMapSize();
    auto tileSize = _mapManager->getTileSize();
    _tileMapWidth = mapSize.width * tileSize.width;
    _tileMapHeight = mapSize.height * tileSize.height;

}

void GameUI::initReinforcementSelectPanel()
{
    auto gameMainPanel = _gameMainUI->getChildByName("Panel_GameMain");
    auto reinforcementSelectPanel = gameMainPanel->getChildByName("Panel_ReinforcementSelectPanel");
    auto selectEnchanterButton = reinforcementSelectPanel->getChildByName<Button*>("Button_SelectEnchanter");
    auto selectArcherButton = reinforcementSelectPanel->getChildByName<Button*>("Button_SelectArcher");
    auto selectBarbarianButton = reinforcementSelectPanel->getChildByName<Button*>("Button_SelectBarbarian");
    auto selectArcherTowerButton = reinforcementSelectPanel->getChildByName<Button*>("Button_SelectArcherTower");
    auto selectEnchanterTowerButton = reinforcementSelectPanel->getChildByName<Button*>("Button_SelectEnchanterTower");
    auto selectBalloonButton = reinforcementSelectPanel->getChildByName<Button*>("Button_SelectBalloon");
    auto selectGargButton = reinforcementSelectPanel->getChildByName<Button*>("Button_SelectGarg");

    auto reinforceConfig = GameConfigManager::getInstance()->getReinforceConfigBy(ForceType::Player);
    initSelectReinforcementButton(selectEnchanterButton, reinforceConfig->enchanterTemplateName, GameObjectType::Npc, reinforceConfig->enchanterReinforceCount);
    initSelectReinforcementButton(selectArcherButton, reinforceConfig->archerTemplateName, GameObjectType::Npc, reinforceConfig->archerReinforceCount);
    initSelectReinforcementButton(selectBarbarianButton, reinforceConfig->barbarianTemplateName, GameObjectType::Npc, reinforceConfig->barbarianReinforceCount);
    initSelectReinforcementButton(selectArcherTowerButton, reinforceConfig->archerTowerTemplateName, GameObjectType::Building, 1);
    initSelectReinforcementButton(selectEnchanterTowerButton, reinforceConfig->enchanterTowerTemplateName, GameObjectType::Building, 1);
    initSelectReinforcementButton(selectBalloonButton, reinforceConfig->balloonTemplateName, GameObjectType::Npc, reinforceConfig->balloonReinforceCount);
    initSelectReinforcementButton(selectGargButton, reinforceConfig->gargTemplateName, GameObjectType::Npc, reinforceConfig->gargReinforceCount);

    disableAllReinforcementButtons();

    auto turnToUpgradeButton = reinforcementSelectPanel->getChildByName<Button*>("Button_TurnToUpgradePanel");
    turnToUpgradeButton->addTouchEventListener(CC_CALLBACK_2(GameUI::onTurnToUpgradePanel, this));
}

void GameUI::initSelectReinforcementButton(Button* button, const string& reinforcementTemplateName, GameObjectType gameObjectType, int reinforcmentCount)
{
    auto label = button->getChildByName<Text*>("Text_Number");
    label->setString(StringUtils::format("X%d", reinforcmentCount));

    button->addTouchEventListener(CC_CALLBACK_2(GameUI::onSelectReinforcementButtonTouched, this));
    _onSelectReinforcementButtonTouchEventMap[button] = CC_CALLBACK_0(GameUI::onCreateReinforcement, 
        this, 
        reinforcementTemplateName, 
        gameObjectType, 
        reinforcmentCount);

    _reinforcementButtonList.push_back(button);
    _reinforceTemplateNameToButtonMap[reinforcementTemplateName] = button;

    createNextRankAt(button, REINFORCEMENT_RANK);
}

void GameUI::onSelectReinforcementButtonTouched(Ref* sender, Widget::TouchEventType touchType)
{
    if (touchType == Widget::TouchEventType::ENDED)
    {
        auto forceData = _forceManager->getForceDataBy(ForceType::Player);
        if (forceData.reinforcePoint <= 0)
        {
            return;
        }

        SoundManager::getInstance()->playUIEffect(UIEffectType::ButtonClick);

        auto iter = _onSelectReinforcementButtonTouchEventMap.find(sender);
        if (iter != _onSelectReinforcementButtonTouchEventMap.end())
        {
            iter->second();
        }
    }
}

void GameUI::onCreateReinforcement(const string& reinforcementTempalteName, GameObjectType gameObjectType, int reinforcementCount)
{
    if (gameObjectType == GameObjectType::Building)
    {
        if (!_gameWorld->_isPlayerHoldingBuilding())
        {
            _gameWorld->_createGameObject(gameObjectType, ForceType::Player, reinforcementTempalteName, _cursorPoint);
        }
    }
    else
    {
        _gameWorld->_createReinfoecement(ForceType::Player, reinforcementTempalteName, reinforcementCount);

        _forceManager->onPlayerReinforcePointReduce();
        onUpdateReinforcePresent();

        SoundManager::getInstance()->playUIEffect(UIEffectType::TrainFinished);
    }
}

void GameUI::onTurnToUpgradePanel(Ref* sender, Widget::TouchEventType touchType)
{
    if (touchType == Widget::TouchEventType::ENDED)
    {
        auto gameMainPanel = _gameMainUI->getChildByName("Panel_GameMain");
        auto upgradePanel = gameMainPanel->getChildByName("Panel_UpgradePanel");
        auto reinforcementSelectPanel = gameMainPanel->getChildByName("Panel_ReinforcementSelectPanel");

        upgradePanel->setVisible(true);
        reinforcementSelectPanel->setVisible(false);
    }
}

void GameUI::update(float deltaTime)
{
    if (_debugInfoLayer->isVisible())
    {
        auto& debugInfo = GameWorldCallBackFunctionsManager::getInstance()->_getDebugInfo();
        _debugInfoLayer->updateInfo(debugInfo);
    }

    updateMinimap();
    updateTechnologyPoint();
    updateGamePassTime();
    updateUpgradePanelTips();
    updateUpgradeButtonStatus();
}

void GameUI::onUpdateReinforcePresent()
{
    auto forceData = _forceManager->getForceDataBy(ForceType::Player);
    int reinforcePoint = forceData.reinforcePoint;

    _askReinforceButton->setTitleText(StringUtils::format("%d", reinforcePoint));

    if (reinforcePoint <= 0)
    {
        disableAllReinforcementButtons();
    }
    else
    {
        enableAllReinforcementButtons();
    }
}

void GameUI::onReinforceButtonSparkMove()
{
    auto buttonSize = _askReinforceButton->getContentSize();
    auto sparkSize = _reinforceButtonSpark->getContentSize();

    _reinforceButtonSpark->setPosition(-sparkSize.width / 2.0f, 0.0f);
    _reinforceButtonSpark->runAction(MoveBy::create(1.0f, Vec2(buttonSize.width + sparkSize.width, 0.0f)));
}

void GameUI::syncCursorPoint(const Vec2& cursorPoint)
{
    _cursorPoint = cursorPoint;
}

void GameUI::onMouseMove(EventCustom* eventCustom)
{
    Vec2* cursorPoint = (Vec2*)eventCustom->getUserData();
    syncCursorPoint(*cursorPoint);
}

bool GameUI::isCursorInGameMainUI()
{
    bool result = false;

    auto gameMainPanel = _gameMainUI->getChildByName("Panel_GameMain");
    if (gameMainPanel->getBoundingBox().containsPoint(_cursorPoint))
    {
        result = true;
    }

    return result;
}

void GameUI::disableAllReinforcementButtons()
{
    for (auto& button : _reinforcementButtonList)
    {
        button->setBright(false);
        button->setEnabled(false);
    }
}

void GameUI::enableAllReinforcementButtons()
{
    for (auto& button : _reinforcementButtonList)
    {
        button->setBright(true);
        button->setEnabled(true);
    }
}

void GameUI::updateMinimap()
{
    _minimapDrawNode->clear();

    auto& gameObjectMap = GameObjectManager::getInstance()->getGameObjectMap();
    for (auto& gameObjectIter : gameObjectMap)
    {
        Color4F color;
        auto gameObject = gameObjectIter.second;
        if (gameObject->getForceType() == ForceType::Player)
        {
            if (gameObject->isSelected())
            {
                color = Color4F(1.0f, 1.0f, 1.0f, 1.0f);
            }
            else
            {
                color = g_setting.playerForceColor;
            }
        }
        else
        {
            color = g_setting.aiForceColor;
        }

        auto gameObjectInTileMapPosition = gameObject->getPosition();
        auto gameObjectInMinimapPosition = Vec2(gameObjectInTileMapPosition.x / _tileMapWidth * _minimapWidth, 
            gameObjectInTileMapPosition.y / _tileMapHeight * _minimapHeight);
        if (gameObject->getGameObjectType() == GameObjectType::Building)
        {
            _minimapDrawNode->drawSolidRect(Vec2(gameObjectInMinimapPosition.x - 2.0f, gameObjectInMinimapPosition.y - 2.0f),
                Vec2(gameObjectInMinimapPosition.x + 2.0f, gameObjectInMinimapPosition.y + 2.0f),
                color
                );
        }
        else if (gameObject->getGameObjectType() == GameObjectType::Npc)
        {
            _minimapDrawNode->drawDot(gameObjectInMinimapPosition, 1.0f, color);
        }
    }


    auto mapPosition = _mapManager->getMapPosition();
    auto mapScale = _mapManager->getMapScale();
    auto visibileSize = Director::getInstance()->getVisibleSize();

    Size minimapScreenBoxSize(visibileSize.width / _tileMapWidth * _minimapWidth / mapScale, 
        visibileSize.height / _tileMapHeight * _minimapHeight / mapScale);

    Vec2 minimapScreenBoxPosition(-mapPosition.x / (_tileMapWidth * mapScale) * _minimapWidth, 
        -mapPosition.y / (_tileMapHeight * mapScale) * _minimapHeight);

    _minimapDrawNode->drawRect(minimapScreenBoxPosition, 
        minimapScreenBoxPosition + minimapScreenBoxSize, 
        Color4F(1.0f, 1.0f, 1.0f, 1.0f));
}

void GameUI::onMinimapTouched(Ref* sender, Widget::TouchEventType touchType)
{
    if (touchType == Widget::TouchEventType::ENDED)
    {
        auto widget = static_cast<Widget*>(sender);
        auto touchPosition = widget->convertToNodeSpace(widget->getTouchEndPosition());
        auto widgetSize = widget->getContentSize();

        float mapScale = _mapManager->getMapScale();
        auto visibleSize = Director::getInstance()->getVisibleSize();

        Vec2 tileMapNewPosition(touchPosition.x / widgetSize.width * _tileMapWidth * mapScale - visibleSize.width / 2.0f,
            touchPosition.y / widgetSize.height * _tileMapHeight * mapScale - visibleSize.height / 2.0f);

        _mapManager->setTileMapPosition(-tileMapNewPosition);

        if (Director::getInstance()->isPaused())
        {
            updateMinimap();
        }
    }
}

void GameUI::updateTechnologyPoint()
{
    auto gameMainPanel = _gameMainUI->getChildByName("Panel_GameMain");
    auto technologyPointLabel = gameMainPanel->getChildByName<Text*>("Text_TechnologyPoint");

    auto& playerForceData = _forceManager->getForceDataBy(ForceType::Player);
    technologyPointLabel->setString(StringUtils::format("%d", playerForceData.technologyPoint));
}

void GameUI::updateGamePassTime()
{
    static float gameStartTime = ::timeGetTime() / 1000.0f;
    float timeDelta = ::timeGetTime() / 1000.0f - gameStartTime;
    time_t passSecond(timeDelta);

    tm* passTime = gmtime(&passSecond);

    auto gameMainPanel = _gameMainUI->getChildByName("Panel_GameMain");
    auto passTimeLabel = gameMainPanel->getChildByName<Text*>("Text_GameTime");
    passTimeLabel->setString(StringUtils::format("%02d:%02d:%02d", passTime->tm_hour, passTime->tm_min, passTime->tm_sec));
}

void GameUI::initUpgradePanel()
{
    auto gameMainPanel = _gameMainUI->getChildByName("Panel_GameMain");
    auto upgradePanel = gameMainPanel->getChildByName("Panel_UpgradePanel");
    upgradePanel->setVisible(false);

    _upgradeTipsPanel = upgradePanel->getChildByName<Layout*>("Panel_Tips");
    _upgradeTipsPanel->setVisible(false);

    auto turnToSelectReinforceButton = upgradePanel->getChildByName<Button*>("Button_TurnToSelectReinforcePanel");
    turnToSelectReinforceButton->addTouchEventListener(CC_CALLBACK_2(GameUI::onTurnToSelectReinforcePanel, this));

    auto reinforcementConfig = GameConfigManager::getInstance()->getReinforceConfigBy(ForceType::Player);

    auto upgradeEnchanterButton = upgradePanel->getChildByName<Button*>("Button_UpgradeEnchanter");
    auto upgradeArcherButton = upgradePanel->getChildByName<Button*>("Button_UpgradeArcher");
    auto upgradeBarbarianButton = upgradePanel->getChildByName<Button*>("Button_UpgradeBarbarian");
    auto upgradeArcherTowerButton = upgradePanel->getChildByName<Button*>("Button_UpgradeArcherTower");
    auto upgradeEnchanterTowerButton = upgradePanel->getChildByName<Button*>("Button_UpgradeEnchanterTower");
    auto upgradeBalloonButton = upgradePanel->getChildByName<Button*>("Button_UpgradeBalloon");
    auto upgradeGargButton = upgradePanel->getChildByName<Button*>("Button_UpgradeGarg");

    initUpgradeButton(upgradeEnchanterButton, reinforcementConfig->enchanterTemplateName);
    initUpgradeButton(upgradeArcherButton, reinforcementConfig->archerTemplateName);
    initUpgradeButton(upgradeBarbarianButton, reinforcementConfig->barbarianTemplateName);
    initUpgradeButton(upgradeArcherTowerButton, reinforcementConfig->archerTowerTemplateName);
    initUpgradeButton(upgradeEnchanterTowerButton, reinforcementConfig->enchanterTowerTemplateName);
    initUpgradeButton(upgradeBalloonButton, reinforcementConfig->balloonTemplateName);
    initUpgradeButton(upgradeGargButton, reinforcementConfig->gargTemplateName);
}

void GameUI::initUpgradeButton(Button* button, const string& gameObjectTemplateName)
{
    auto buttonSize = button->getContentSize();

    auto nextRankSprite = createNextRankAt(button, UPGRADE_NEXT_RANK);
    nextRankSprite->setSpriteFrame("Level1Rank.png");

    auto upgradeProgressBarBG = Sprite::create("HPBarBackground.png");
    auto progressBarBGSize = upgradeProgressBarBG->getContentSize();
    upgradeProgressBarBG->setPosition(buttonSize.width / 2.0f, -4.0f);
    upgradeProgressBarBG->setScale(0.3f);
    upgradeProgressBarBG->setCascadeOpacityEnabled(true);
    upgradeProgressBarBG->setVisible(false);
    button->addChild(upgradeProgressBarBG, 1, UPGRADE_PROGRESS_BAR_BG);

    auto upgradeProgressSprite = Sprite::create("PlayerHPBar.png");
    auto upgradeProgressBar = ProgressTimer::create(upgradeProgressSprite);
    upgradeProgressBar->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
    upgradeProgressBar->setPercentage(0.0f);
    upgradeProgressBar->setType(ProgressTimer::Type::BAR);
    upgradeProgressBarBG->addChild(upgradeProgressBar, 1, UPGRADE_PROGRESS_BAR);

    button->addTouchEventListener(CC_CALLBACK_2(GameUI::onUpgradeButtonTouched, this));

    _upgradeButtonList.push_back(button);
    _upgradeButtonToTemplateNameMap[button] = gameObjectTemplateName;
}

void GameUI::onTurnToSelectReinforcePanel(Ref* sender, Widget::TouchEventType touchType)
{
    if (touchType == Widget::TouchEventType::ENDED)
    {
        auto gameMainPanel = _gameMainUI->getChildByName("Panel_GameMain");
        auto upgradePanel = gameMainPanel->getChildByName("Panel_UpgradePanel");
        auto reinforcementSelectPanel = gameMainPanel->getChildByName("Panel_ReinforcementSelectPanel");

        upgradePanel->setVisible(false);
        reinforcementSelectPanel->setVisible(true);
    }
}

void GameUI::onUpgradeButtonTouched(Ref* sender, Widget::TouchEventType touchType)
{
    if (touchType == Widget::TouchEventType::ENDED)
    {
        SoundManager::getInstance()->playUIEffect(UIEffectType::ButtonClick);

        auto forceManager = ForceManager::getInstance();
        auto gameConfigManager = GameConfigManager::getInstance();
        auto templateName = _upgradeButtonToTemplateNameMap[sender];

        int gameObjectLevel = forceManager->getGameObjectLevel(templateName);
        int gameObjectNextLevel = gameObjectLevel + 1;
        if (gameObjectNextLevel > MAX_LEVEL)
        {
            return;
        }

        int playerTechnologyPoint = forceManager->getForceDataBy(ForceType::Player).technologyPoint;
        int costTechnologyPoint = gameConfigManager->getGameObjectLevelConfig(templateName, gameObjectNextLevel)->costTechnologyPoint;
        if (playerTechnologyPoint < costTechnologyPoint)
        {
            return;
        }

        forceManager->costTechnologyPoint(ForceType::Player, costTechnologyPoint);

        auto button = static_cast<Button*>(sender);
        auto upgradeProgressBarBG = button->getChildByName(UPGRADE_PROGRESS_BAR_BG);
        upgradeProgressBarBG->setVisible(true);
        
        auto upgradeProgressBar = upgradeProgressBarBG->getChildByName<ProgressTimer*>(UPGRADE_PROGRESS_BAR);
        upgradeProgressBar->setPercentage(0.0f);
        upgradeProgressBar->setMidpoint(Vec2(0.0f, 0.5f));
        upgradeProgressBar->setBarChangeRate(Vec2(1.0f, 0.0f));
        auto progressToAction = ProgressTo::create(UPGRADE_TIME * gameObjectNextLevel, 100.0f);
        auto callBackFunction = CallFunc::create(CC_CALLBACK_0(GameUI::onUpgradeSuccess, this, button, gameObjectNextLevel));
        auto sequenceAction = Sequence::create(progressToAction, callBackFunction, nullptr);
        upgradeProgressBar->runAction(sequenceAction);

        button->setBright(false);
        button->setEnabled(false);
    }
}

void GameUI::onUpgradeSuccess(Button* sender, int newLevel)
{
    sender->setBright(true);
    sender->setEnabled(true);

    auto upgradeProgressBarBG = sender->getChildByName(UPGRADE_PROGRESS_BAR_BG);
    upgradeProgressBarBG->setVisible(false);

    int nextUpgradeLevel = newLevel + 1;
    if (nextUpgradeLevel <= MAX_LEVEL)
    {
        auto upgradeNextRank = sender->getChildByName<Sprite*>(UPGRADE_NEXT_RANK);
        upgradeNextRank->setSpriteFrame(StringUtils::format("Level%dRank.png", nextUpgradeLevel));
    }
    else
    {
        sender->setBright(false);
        sender->setEnabled(false);
    }

    auto templateName = _upgradeButtonToTemplateNameMap[sender];
    ForceManager::getInstance()->setGameObjectLevel(templateName, newLevel);

    auto reinforcementButton = _reinforceTemplateNameToButtonMap[templateName];
    auto reinforcementRank = reinforcementButton->getChildByName<Sprite*>(REINFORCEMENT_RANK);
    reinforcementRank->setSpriteFrame(StringUtils::format("Level%dRank.png", newLevel));
}

void GameUI::updateUpgradePanelTips()
{
    Button* cursorOverButton = nullptr;
    for (auto button : _upgradeButtonList)
    {
        auto parentNode = button->getParent();
        auto buttonSize = button->getContentSize();
        auto buttonWorldPosition = parentNode->convertToWorldSpace(button->getPosition());

        auto buttonRect = Rect(buttonWorldPosition.x - buttonSize.width / 2.0f, 
            buttonWorldPosition.y - buttonSize.height / 2.0f, 
            buttonSize.width, 
            buttonSize.height);

        if (buttonRect.containsPoint(_cursorPoint))
        {
            cursorOverButton = button;
            break;
        }
    }

    if (!cursorOverButton)
    {
        _upgradeTipsPanel->setVisible(false);
        return;
    }

    auto gameObjectTemplateName = _upgradeButtonToTemplateNameMap[cursorOverButton];
    auto gameObjectLevel = ForceManager::getInstance()->getGameObjectLevel(gameObjectTemplateName);
    if (gameObjectLevel < MAX_LEVEL)
    {
        auto playerTechnologyPoint = ForceManager::getInstance()->getForceDataBy(ForceType::Player).technologyPoint;
        auto gameObjectLevelConfig = GameConfigManager::getInstance()->getGameObjectLevelConfig(gameObjectTemplateName, gameObjectLevel + 1);

        if (playerTechnologyPoint >= gameObjectLevelConfig->costTechnologyPoint)
        {
            showNeedTechnologyPoint(gameObjectLevelConfig->costTechnologyPoint, Color3B::BLUE);
        }
        else
        {
            showNeedTechnologyPoint(gameObjectLevelConfig->costTechnologyPoint, Color3B::RED);
        }
    }
    else
    {
        _upgradeTipsPanel->setVisible(false);
    }
}

void GameUI::showNeedTechnologyPoint(int needTechnologyPoint, const Color3B& color)
{
    _upgradeTipsPanel->setVisible(true);
    auto technologyPointLabel = _upgradeTipsPanel->getChildByName<Text*>("Text_NeedTechnologyPoint");
    technologyPointLabel->setString(StringUtils::format("%d", needTechnologyPoint));
    technologyPointLabel->setColor(color);
}

void GameUI::updateUpgradeButtonStatus()
{
    auto forceManager = ForceManager::getInstance();
    auto playerForceData = forceManager->getForceDataBy(ForceType::Player);

    auto playerTechnologyPoint = playerForceData.technologyPoint;
    for (auto button : _upgradeButtonList)
    {
        auto gameObjectTemplateName = _upgradeButtonToTemplateNameMap[button];
        auto gameObjectLevel = forceManager->getGameObjectLevel(gameObjectTemplateName);
        if (gameObjectLevel < MAX_LEVEL)
        {
            auto gameObjectLevelConfig = GameConfigManager::getInstance()->getGameObjectLevelConfig(gameObjectTemplateName, gameObjectLevel + 1);
            CCASSERT(gameObjectLevelConfig, "");

            if (playerTechnologyPoint >= gameObjectLevelConfig->costTechnologyPoint &&
                !isUpgrading(button))
            {
                button->setBright(true);
                button->setEnabled(true);
            }
            else
            {
                button->setBright(false);
                button->setEnabled(false);
            }
        }
        else
        {
            button->setBright(false);
            button->setEnabled(false);
        }
    }
}

Sprite* GameUI::createNextRankAt(Button* button, const string& childName)
{
    auto buttonSize = button->getContentSize();

    auto nextRankSprite = Sprite::create();
    nextRankSprite->setPosition(buttonSize.width / 2.0f, 0.0f);
    nextRankSprite->setScale(0.4f);

    button->addChild(nextRankSprite, 1, childName);

    return nextRankSprite;
}

bool GameUI::isUpgrading(Button* button)
{
    bool result = false;

    auto upgradeProgressBarBG = button->getChildByName(UPGRADE_PROGRESS_BAR_BG);
    result = upgradeProgressBarBG->isVisible();

    return result;
}

bool GameUI::isShowDebugLayer()
{
    return _debugInfoLayer->isVisible();
}

void GameUI::setDebugLayerShowStatus(bool isShow)
{
    _debugInfoLayer->setVisible(isShow);
}

void GameUI::initGameEndTips()
{
    auto gameMainPanel = _gameMainUI->getChildByName("Panel_GameMain");
    _winTips = gameMainPanel->getChildByName<Text*>("Text_WinTips");
    _lostTips = gameMainPanel->getChildByName<Text*>("Text_LostTips");

    _winTips->setVisible(false);
    _lostTips->setVisible(false);
}

void GameUI::showGameWinTips()
{
    _winTips->setVisible(true);
    _lostTips->setVisible(false);
}

void GameUI::showGameLostTips()
{
    _lostTips->setVisible(true);
    _winTips->setVisible(false);
}

void GameUI::initOptionPanel()
{
    auto gameMainPanel = _gameMainUI->getChildByName("Panel_GameMain");
    auto openOptionPanelButton = gameMainPanel->getChildByName<Button*>("Button_OpenOptionPanel");
    openOptionPanelButton->addTouchEventListener(CC_CALLBACK_2(GameUI::onOpenOptionPanel, this));

    _optionPanel = _gameMainUI->getChildByName<Layout*>("Panel_Option");
    _optionPanel->setVisible(false);

    auto continueButton = _optionPanel->getChildByName<Button*>("Button_Continue");
    auto gotoSelectStageSceneButton = _optionPanel->getChildByName<Button*>("Button_GotoSelectStageScene");
    auto gotoMenuSceneButton = _optionPanel->getChildByName<Button*>("Button_GotoMenuScene");
    auto backToWindowsButton = _optionPanel->getChildByName<Button*>("Button_BackToWindows");

    continueButton->addTouchEventListener(CC_CALLBACK_2(GameUI::onGameContinue, this));
    gotoSelectStageSceneButton->addTouchEventListener(CC_CALLBACK_2(GameUI::onGotoSelectStageScene, this));
    gotoMenuSceneButton->addTouchEventListener(CC_CALLBACK_2(GameUI::onGotoMenuScene, this));
    backToWindowsButton->addTouchEventListener(CC_CALLBACK_2(GameUI::onBackToWindows, this));
}

void GameUI::onOpenOptionPanel(Ref* sender, Widget::TouchEventType type)
{
    if (type == Widget::TouchEventType::ENDED)
    {
        SoundManager::getInstance()->playUIEffect(UIEffectType::ButtonClick);
        _optionPanel->setVisible(true);
    }
}

void GameUI::onGameContinue(Ref* sender, Widget::TouchEventType type)
{
    if (type == Widget::TouchEventType::ENDED)
    {
        SoundManager::getInstance()->playUIEffect(UIEffectType::ButtonClick);
        _optionPanel->setVisible(false);
    }
}

void GameUI::onGotoSelectStageScene(Ref* sender, Widget::TouchEventType type)
{
    if (type == Widget::TouchEventType::ENDED)
    {
        SoundManager::getInstance()->stopAll();

        auto selectStageScene = SelectStageScene::createScene();
        Director::getInstance()->replaceScene(selectStageScene);
    }
}

void GameUI::onGotoMenuScene(Ref* sender, Widget::TouchEventType type)
{
    if (type == Widget::TouchEventType::ENDED)
    {
        SoundManager::getInstance()->stopAll();

        auto menuScene = MenuScene::createScene();
        Director::getInstance()->replaceScene(menuScene);
    }
}

void GameUI::onBackToWindows(Ref* sender, Widget::TouchEventType type)
{
    if (type == Widget::TouchEventType::ENDED)
    {
        SoundManager::getInstance()->playUIEffect(UIEffectType::ButtonClick);

        WindowsHelper::getInstance()->uninitInstane();
        Director::getInstance()->end();
    }
}

void GameUI::clear()
{
    Director::getInstance()->getEventDispatcher()->removeCustomEventListeners("MouseMove");
}
