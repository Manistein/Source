#pragma once

class MapManager;

class GameWorld : public Node
{
public:
    ~GameWorld();
	CREATE_FUNC(GameWorld);

    MapManager* _mapManager = nullptr;
private:
    bool init() override;
    void update(float deltaTime) override;
    void onMouseScroll(Event* event);
};