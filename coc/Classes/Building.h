#pragma once

enum class BuildingStatus
{
    Invalid = 0,

    PrepareToBuild,
    BeingBuilt,
    Working,
    Destory,

    Total
};

class Building : public GameObject
{
public:
    static Building* create(ForceType forceType, const string& buildingTemplateName, const Vec2& position, int uniqueID);

    bool isReadyToRemove() override;
    void clearDebugDraw() override;

    void update(float delta) override;
    void updateStatus(BuildingStatus buildingStatus);
    BuildingStatus getBuildingStatus();

    bool canBuild();
private:
    bool init(ForceType forceType, const string& buildingTemplateName, const Vec2& position, int uniqueID);

    void initBuildingStatusSprites(const string& buildingTemplateName);
    Sprite* createBuildingStatusSprite(const string& buildingTemplateName, BuildingStatus buildingStatus, int opacity = 255);

    void initBottomGridSprites(const string& buildingTemplateName);
    void initHPBar();
    void initBeingBuiltProgressBar();
    void initBattleData(const string& buildingTemplateName);
    void clear();

    void onPrepareToRemove() override;
    void debugDraw() override;

    void followCursorInPrepareToBuildStatus();
    void updateBottomGridTextureInPrepareToBuildStatus();

    void showBeingBuiltProgressBar();
    void hideBeingBuiltProgressBar();
    void onConstructionComplete();
    void updateBeingBuiltProgressBar(float delta);

    void addToRemoveQueue();

    map<BuildingStatus, Sprite*> _buildingStatusSpriteMap;
    vector<Sprite*> _bottomGridSpritesList;
    Vec2 _bottomGridsPlaneCenterPositionInLocalSpace; // 建筑物底部面片的中心位置

    BuildingStatus _buildingStatus;
    bool _canBuild = true;

    float _buildingTimeBySecond = 0.0f;
    float _passTimeBySecondInBeingBuiltStatus = 0.0f;
    ui::LoadingBar* _beingBuildProgressBar = nullptr;
};