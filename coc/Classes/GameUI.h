#pragma once

class DebugInfoLayer;
class GameWorld;
class MapManager;
class GameWorldCallBackFunctionsManager;
class ForceManager;

const float UPGRADE_TIME = 10.0f;

class GameUI : public Node
{
public:
    CREATE_FUNC(GameUI);

    void onMouseMove(EventCustom* eventCustom);

    bool isCursorInGameMainUI();
    void disableAllReinforcementButtons();
    void enableAllReinforcementButtons();

    void onUpdateReinforcePresent();
private:
    bool init() override;
    void initReinforcePresent();
    void initMiniMapPresent();
    void initReinforcementSelectPanel();
    void initSelectReinforcementButton(Button* button, const string& reinforcementTemplateName, GameObjectType gameObjectType, int reinforcmentCount);
    void onSelectReinforcementButtonTouched(Ref* sender, Widget::TouchEventType touchType);
    void onCreateReinforcement(const string& reinforcementTempalteName, GameObjectType gameObjectType, int reinforcementCount);
    void onTurnToUpgradePanel(Ref* sender, Widget::TouchEventType touchType);
    map<Ref*, std::function<void()>> _onSelectReinforcementButtonTouchEventMap;
    map<string, Button*> _reinforceTemplateNameToButtonMap;

    void initUpgradePanel();
    void initUpgradeButton(Button* button, const string& gameObjectTemplateName);
    void onTurnToSelectReinforcePanel(Ref* sender, Widget::TouchEventType touchType);
    void onUpgradeButtonTouched(Ref* sender, Widget::TouchEventType touchType);
    void onUpgradeSuccess(Button* sender, int newLevel);
    Layout* _upgradeTipsPanel = nullptr;
    map<Ref*, string> _upgradeButtonToTemplateNameMap;

    void update(float deltaTime) override;
    void onReinforceButtonSparkMove();
    void updateMinimap();
    void onMinimapTouched(Ref* sender, Widget::TouchEventType touchType);

    void updateTechnologyPoint();
    void updateGamePassTime();

    void syncCursorPoint(const Vec2& cursorPoint);

    Sprite* createNextRankAt(Button* button, const string& childName);

    GameWorldCallBackFunctionsManager* _gameWorld = nullptr;
    ForceManager* _forceManager = nullptr;
    DebugInfoLayer* _debugInfoLayer = nullptr;
    Node* _gameMainUI = nullptr;
    Button* _askReinforceButton = nullptr;
    Sprite* _reinforceButtonSpark = nullptr;

    DrawNode* _minimapDrawNode = nullptr;
    ImageView* _minimapImage = nullptr;
    MapManager* _mapManager = nullptr;
    float _tileMapWidth = 0.0f;
    float _tileMapHeight = 0.0f;
    float _minimapWidth = 0.0f;
    float _minimapHeight = 0.0f;

    Vec2 _cursorPoint;

    vector<Button*> _reinforcementButtonList;
    vector<Button*> _upgradeButtonList;
};