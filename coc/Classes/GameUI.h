#pragma once

class DebugInfoLayer;
class GameWorld;

class GameUI : public Node
{
public:
    CREATE_FUNC(GameUI);

    void registerGameWorldCallBackFunctions(GameWorld* gameWorld);
    void onMouseMove(EventCustom* eventCustom);

    DebugInfoLayer* _debugInfoLayer = nullptr;
private:
    bool init() override;
    void update(float deltaTime) override;

    void syncCursorPoint(const Vec2& cursorPoint);

    Vec2 _cursorPoint;

    struct GameWorldCallBackFunctions 
    {
        std::function<DebugInfo(TileMapLayerType)> getDebugInfo;
    };
    GameWorldCallBackFunctions _gameWorld;
};