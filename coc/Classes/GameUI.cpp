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
#include "SoundManager.h"

bool GameUI::init()
{
    if (!Node::init())
    {
        return false;
    }

    _debugInfoLayer = DebugInfoLayer::create();
    addChild(_debugInfoLayer);

    _gameMainUI = CSLoader::createNode("MainScene.csb");
    addChild(_gameMainUI);

    _gameWorld = GameWorldCallBackFunctionsManager::getInstance();
    _forceManager = ForceManager::getInstance();

    initReinforcePresent();
    initMiniMapPresent();
    initReinforcementSelectPanel();

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

    auto reinforceConfig = GameConfigManager::getInstance()->getReinforceConfigBy(ForceType::Player);
    initSelectReinforcementButton(selectEnchanterButton, reinforceConfig->enchanterTemplateName, GameObjectType::Npc, reinforceConfig->enchanterReinforceCount);
    initSelectReinforcementButton(selectArcherButton, reinforceConfig->archerTemplateName, GameObjectType::Npc, reinforceConfig->archerReinforceCount);
    initSelectReinforcementButton(selectBarbarianButton, reinforceConfig->barbarianTemplateName, GameObjectType::Npc, reinforceConfig->barbarianReinforceCount);
    initSelectReinforcementButton(selectArcherTowerButton, reinforceConfig->archerTowerTemplateName, GameObjectType::Building, 1);
    initSelectReinforcementButton(selectEnchanterTowerButton, reinforceConfig->enchanterTowerTemplateName, GameObjectType::Building, 1);

    disableAllReinforcementButtons();
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
        _gameWorld->_createNpcAroundBaseCamp(ForceType::Player, reinforcementTempalteName, reinforcementCount);

        _forceManager->onPlayerReinforcePointReduce();
        onUpdateReinforcePresent();

        SoundManager::getInstance()->playUIEffect(UIEffectType::TrainFinished);
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
                color = Color4F(248.0f / 255.0f, 200.0f / 255.0f, 40.0f / 255.0f, 1.0f);
            }
        }
        else
        {
            color = Color4F(164.0f / 255.0f, 72.0f / 255.0f, 192.0f / 255.0f, 1.0f);
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
    }
}

void GameUI::updateTechnologyPoint()
{
    auto gameMainPanel = _gameMainUI->getChildByName("Panel_GameMain");
    auto technologyPointLabel = gameMainPanel->getChildByName<Text*>("Text_TechnologyPoint");

    auto& playerForceData = _forceManager->getForceDataBy(ForceType::Player);
    technologyPointLabel->setString(StringUtils::format("%d", playerForceData.technologyPoint));
}
