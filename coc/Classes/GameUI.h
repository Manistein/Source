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
    void update(float deltaTime) override;

    void syncCursorPoint(const Vec2& cursorPoint);

    DebugInfoLayer* _debugInfoLayer = nullptr;
    Node* _gameMainUI = nullptr;
    Vec2 _cursorPoint;
};