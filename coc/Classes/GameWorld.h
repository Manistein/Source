#pragma once

class MapManager;

class GameWorld : public Node
{
public:
    ~GameWorld();
	CREATE_FUNC(GameWorld);

    Vec2 getCursorPosition();
private:
    bool init() override;
    void update(float deltaTime) override;
    void updateMapPosition();
    void updateMapScale();

    void onMouseMove(Event* event);
    void onMouseDown(Event* event);
    void onMouseUp(Event* event);
    void onMouseScroll(Event* event);

    MapManager* _mapManager = nullptr;
};