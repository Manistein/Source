#pragma once

class DebugInfoLayer;

class GameUI : public Node
{
public:
    CREATE_FUNC(GameUI);

    void syncCursorPosition(const Vec2& cursorPosition);

    DebugInfoLayer* _debugInfoLayer = nullptr;
private:
    bool init() override;
    void update(float deltaTime) override;

    Vec2 _cursorPosition;
};