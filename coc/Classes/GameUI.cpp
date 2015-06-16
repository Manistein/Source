#include "Base.h"
#include "MapManager.h"
#include "GameUI.h"
#include "DebugInfoLayer.h"
#include "GameObject.h"
#include "GameWorld.h"
#include "GameWorldCallBackFunctionsManager.h"
#include "cocostudio/ActionTimeline/CSLoader.h"
#include "ForceManager.h"
#include "GameObjectManager.h"

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

    initReinforcePresent();
    initMiniMapPresent();

    Director::getInstance()->getEventDispatcher()->addCustomEventListener("MouseMove", CC_CALLBACK_1(GameUI::onMouseMove, this));

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
        ProgressTo::create(PLAYER_REINFORCE_TIME_INTERVAL, 100),
        CallFunc::create(CC_CALLBACK_0(ForceManager::onPlayerReinforcePointIncrease, ForceManager::getInstance())),
        CallFunc::create(CC_CALLBACK_0(ProgressTimer::setPercentage, reinforceProgressBar, 0.0f)),
        CallFunc::create(CC_CALLBACK_0(GameUI::onUpdateReinforceCount, this)),
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


    auto gameWorld = GameWorldCallBackFunctionsManager::getInstance();
    _mapManager = gameWorld->_getMapManager();

    _minimapWidth = _minimapImage->getContentSize().width;
    _minimapHeight = _minimapImage->getContentSize().height;

    auto mapSize = _mapManager->getMapSize();
    auto tileSize = _mapManager->getTileSize();
    _tileMapWidth = mapSize.width * tileSize.width;
    _tileMapHeight = mapSize.height * tileSize.height;

}

void GameUI::update(float deltaTime)
{
    if (_debugInfoLayer->isVisible())
    {
        auto& debugInfo = GameWorldCallBackFunctionsManager::getInstance()->_getDebugInfo();
        _debugInfoLayer->updateInfo(debugInfo);
    }

    updateMinimap();
}

void GameUI::onUpdateReinforceCount()
{
    auto forceManager = ForceManager::getInstance();
    auto forceData = forceManager->getForceDataBy(ForceType::Player);

    _askReinforceButton->setTitleText(StringUtils::format("%d", forceData.reinforcePoint));
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
            color = Color4F(248.0f / 255.0f, 200.0f / 255.0f, 40.0f / 255.0f, 1.0f);
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
