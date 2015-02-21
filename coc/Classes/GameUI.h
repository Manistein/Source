#pragma once

class GameUI : public Node
{
public:
    CREATE_FUNC(GameUI);

    void setCursorPosition(const Vec2& cursorPosition);
private:
    bool init() override;
    void update(float deltaTime) override;
    void updateCursorPosition();

    Label* _cursorPositionLabel = nullptr;
    Vec2 _cursorPosition;
};