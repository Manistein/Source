#pragma once

class DebugInfoLayer;
class GameWorld;
class MapManager;

class GameUI : public Node
{
public:
    CREATE_FUNC(GameUI);

    void onMouseMove(EventCustom* eventCustom);

    bool isCursorInGameMainUI();
private:
    bool init() override;
    void initReinforcePresent();
    void initMiniMapPresent();

    void update(float deltaTime) override;
    void onUpdateReinforceCount();
    void onReinforceButtonSparkMove();
    void updateMinimap();
    void onMinimapTouched(Ref* sender, Widget::TouchEventType touchType);

    void syncCursorPoint(const Vec2& cursorPoint);

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
};