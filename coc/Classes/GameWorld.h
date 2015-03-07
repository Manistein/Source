#pragma once

class MapManager;
class Npc;

class GameWorld : public Node
{
public:
    ~GameWorld();
	CREATE_FUNC(GameWorld);

    void onMouseRightButtonDownEvent();

    MapManager* _mapManager = nullptr;
private:
    bool init() override;
    void update(float deltaTime) override;
    void onMouseScroll(Event* event);

    Npc* _testNpc = nullptr;
};