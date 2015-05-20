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

    bool canUpdateToWorkingStatus();
private:
    bool init(ForceType forceType, const string& buildingTemplateName, const Vec2& position, int uniqueID);
    void initBuildingStatusSprites(const string& buildingTemplateName);
    void initBottomGridSprites(const string& buildingTemplateName);
    void initHPBar();
    void initBattleData(const string& buildingTemplateName);
    void clear();

    void onPrepareToRemove() override;
    void debugDraw() override;

    void followCursorInPrepareToBuildStatus();
    void updateBottomGridTextureInPrepareToBuildStatus();

    map<BuildingStatus, Sprite*> _buildingStatusSpriteMap;
    vector<Sprite*> _bottomGridSpritesList;
    Vec2 _bottomGridsPlaneCenterPositionInLocalSpace; // 建筑物底部面片的中心位置

    BuildingStatus _buildingStatus;
    bool _canUpdateToWorkingStatus = true;
};