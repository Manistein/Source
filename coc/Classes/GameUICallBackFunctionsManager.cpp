#include "Base.h"
#include "GameObject.h"
#include "GameUICallBackFunctionsManager.h"
#include "GameUI.h"

static GameUICallbackFunctionsManager* s_gameUICallbackFunctionsManager = nullptr;

GameUICallbackFunctionsManager* GameUICallbackFunctionsManager::getInstance()
{
    if (s_gameUICallbackFunctionsManager == nullptr)
    {
        s_gameUICallbackFunctionsManager = new GameUICallbackFunctionsManager();
    }

    return s_gameUICallbackFunctionsManager;
}

void GameUICallbackFunctionsManager::registerCallBackFunctions(GameUI* gameUI)
{
    _onUpdateReinforcePresent = CC_CALLBACK_0(GameUI::onUpdateReinforcePresent, gameUI);
    _showGameWinTips = CC_CALLBACK_0(GameUI::showGameWinTips, gameUI);
    _showGameLostTips = CC_CALLBACK_0(GameUI::showGameLostTips, gameUI);
}
