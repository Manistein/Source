#pragma once

class GameUI;

class GameUICallbackFunctionsManager
{
public:
    static GameUICallbackFunctionsManager* getInstance();
    void registerCallBackFunctions(GameUI* gameUI);

    std::function<void()> _onUpdateReinforcePresent;
private:
    GameUICallbackFunctionsManager(){};
    GameUICallbackFunctionsManager(const GameUICallbackFunctionsManager&);
    GameUICallbackFunctionsManager& operator = (const GameUICallbackFunctionsManager&);
};