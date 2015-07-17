#pragma once

class GameUI;

class GameUICallbackFunctionsManager
{
public:
    static GameUICallbackFunctionsManager* getInstance();
    void registerCallBackFunctions(GameUI* gameUI);

    std::function<void()> _onUpdateReinforcePresent;
    std::function<void()> _showGameWinTips;
    std::function<void()> _showGameLostTips;
private:
    GameUICallbackFunctionsManager(){};
    GameUICallbackFunctionsManager(const GameUICallbackFunctionsManager&);
    GameUICallbackFunctionsManager& operator = (const GameUICallbackFunctionsManager&);
};