#pragma once

class GameWorld;
class GameUI;

class GameScene : public cocos2d::Layer
{
public:
    static cocos2d::Scene* createScene();
    
    CREATE_FUNC(GameScene);

    void onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event) override;
    void onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event) override;
private:
    bool init() override;
    void initNpcResources();
    void update(float deltaTime) override;

    void onMouseMove(Event* event);
    void onMouseDown(Event* event);
    void onMouseUp(Event* event);

    GameWorld* _gameWorld = nullptr;
    GameUI* _gameUI = nullptr;

    Director* _director = nullptr;

    Vec2 _cursorPoint;
    RECT _clientRect;
};