#pragma once

class LoadingScene : public Layer
{
public:
    static cocos2d::Scene* createScene();

    CREATE_FUNC(LoadingScene);
private:
    bool init() override;
    void initGameObjectResources();
    void update(float delta) override;

    Label* _loadingLabel = nullptr;

    int _loadingIndex = 0;
    vector<string> _plistFileNameList;
};