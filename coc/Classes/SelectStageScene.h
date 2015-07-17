#pragma once

enum class DifficultyLevel
{
    Easy,
    Normal,
    Hard
};

class SelectStageScene : public Layer
{
public:
    static cocos2d::Scene* createScene();

    CREATE_FUNC(SelectStageScene);
private:
    bool init() override;
    void initFrontUI();
    void initSelectStageScrollView();

    void onStageCheckBoxTouch(Ref* sender, CheckBox::EventType type, int stageIndex);
    void onSelectDifficultyLevelCheckBoxTouch(Ref* sender, CheckBox::EventType type);
    void onGameStart(Ref* sender, Widget::TouchEventType type);

    Layout* _rootPanel = nullptr;

    Layout* _frontUI = nullptr;
    unordered_map<CheckBox*, DifficultyLevel> _difficultyLevelMap;
    ImageView* _stageTips = nullptr;

    ScrollView* _selectStageScrollView = nullptr;
    vector<CheckBox*> _stageCheckBoxList;
};