#pragma once

class DebugInfoLayer;
class GameWorld;

class GameUI : public Node
{
public:
    CREATE_FUNC(GameUI);

    void onMouseMove(EventCustom* eventCustom);

    bool isCursorInGameMainUI();
private:
    bool init() override;
    void initReinforcePresent();

    void update(float deltaTime) override;
    void onUpdateReinforceCount();
    void onReinforceButtonSparkMove();

    void syncCursorPoint(const Vec2& cursorPoint);

    DebugInfoLayer* _debugInfoLayer = nullptr;
    Node* _gameMainUI = nullptr;
    Button* _askReinforceButton = nullptr;
    Sprite* _reinforceButtonSpark = nullptr;

    Vec2 _cursorPoint;
};