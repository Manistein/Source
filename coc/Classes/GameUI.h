#pragma once

class DebugInfoLayer;
class GameWorld;
class MapManager;
class GameWorldCallBackFunctionsManager;
class ForceManager;

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
    map<Ref*, std::function<void()>> _onSelectReinforcementButtonTouchEventMap;

    void update(float deltaTime) override;
    void onReinforceButtonSparkMove();
    void updateMinimap();
    void onMinimapTouched(Ref* sender, Widget::TouchEventType touchType);

    void syncCursorPoint(const Vec2& cursorPoint);

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
};