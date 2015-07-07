#pragma once

enum class NpcStatus
{
    Invalid = 0,

    Move,
    Stand,
    Attack,
    Die,

    Total
};

enum class FaceDirection
{
    Invalid = 0,

    FaceToEast,
    FaceToNorthEast,
    FaceToNorthWest,
    FaceToSouthEast,
    FaceToSouthWest,
    FaceToWest,

    Total
};

struct MopUpCommand
{
    bool isExecuting = false;
    Vec2 finalPosition;
};

const float HANDLE_ENEMY_IN_ALERT_RANGE_SITUATION_TIME_INTERVAL = 1.0f;
const float SEARCH_ENEMY_COOL_DOWN_TIME_INTERVAL = 0.17f;    //每秒搜索10次

class GameWorldCallBackFunctionsManager;
class Building;

class Npc : public GameObject
{
public:
    ~Npc();

    static Npc* create(ForceType forceType, GameObjectType npcType, const string& templateName, const Vec2& position, int uniqueID);

    void moveTo(const Vec2& targetPosition, bool isAllowEndTileNodeToMoveIn = false); // 如果最后一个参数是true，即便是最后一个节点是障碍物，也是可以去计算寻路路径的
    void setReadyToMoveStatus(bool isReadyToMove);

    NpcStatus getNpcStatus();

    bool isReadyToRemove() override;

    const Vec2& getPosition() const override;
    void initDefenceInBuildingNpcInMapPosition();
    void setAlertRange(float alertRange);
    void setAttackRange(float attackRange);
    void setAttackPower(float attackPower);

    void setMopUpCommand(const MopUpCommand& mopUpCommand);
    bool isExecutingMopUpCommand();

    void setSelected(bool isSelect) override;

    void updatePropertyBy(int level) override;
private:
    bool init(ForceType forceType, GameObjectType npcType, const string& templateName, const Vec2& position, int uniqueID);
    void clear();

    void initAnimates(const string& templateName);
    void initSwitchStatusFunctions();
    void initShadow(const string& templateName);
    void initHPBar(const string& templateName);
    void initBattleData(const string& templateName);
    void initDebugDraw();
    void initSelectedTips(const string& templateName);
    void initTeamIDLabel();

    void update(float delta) override;
    void tryUpdateStatus(NpcStatus newStatus);

    void debugDraw() override;
    void onPrepareToRemove() override;

    void collisionTest();
    void runFightWithEnemyAI(float delta);
    void runDefenceInBuildingAI(float delta);

    bool canSearchEnemy();
    void searchNearbyEnemy();
    GameObject* searchNearestNeedReinforceGameObject();

    bool isEnemyInAttackRange(GameObject* enemy);
    bool isEnemyInAlertRange(GameObject* enemy);
    void updateStatusWhenEnemyInAlertRange(GameObject* enemy);
    void updateStatusWhenEnemyLeaveAlertRange();
    void reinforceOwnSide(GameObject* gameObject);
    void tryChase(GameObject* enemy);

    Vec2 computeArrivePositionBy(GameObject* enemy);
    float getDistanceFrom(GameObject* enemy);
    float _handleEnemyInAlertRangeSituationCoolDownTime = HANDLE_ENEMY_IN_ALERT_RANGE_SITUATION_TIME_INTERVAL;
    float _searchEnemyCoolDownTime = SEARCH_ENEMY_COOL_DOWN_TIME_INTERVAL;

    struct SwitchStatusFunction 
    {
        std::function<void()> switchFunction;
        std::function<bool()> canSwitch;
    };
    SwitchStatusFunction _switchStatusFunctions[(int)NpcStatus::Total][(int)NpcStatus::Total];

    void registerSwitchStatusFunction(SwitchStatusFunction& switchStatusFunctions, std::function<bool()> canSwitch, std::function<void()> switchFunction);

    NpcStatus _oldStatus = NpcStatus::Stand;

    bool canSwitchMoveToMove();
    bool canSwitchMoveToStand();
    bool canSwitchMoveToAttack();
    bool canSwitchMoveToDie();
    void switchMoveToMove();
    void switchMoveToStand();
    void switchMoveToAttack();
    void switchMoveToDie();

    bool canSwitchStandToStand();
    bool canSwitchStandToMove();
    bool canSwitchStandToAttack();
    bool canSwitchStandToDie();
    void switchStandToStand();
    void switchStandToMove();
    void switchStandToAttack();
    void switchStandToDie();

    bool canSwitchAttackToAttack();
    bool canSwitchAttackToMove();
    bool canSwitchAttackToStand();
    bool canSwitchAttackToDie();
    void switchAttackToAttack();
    void switchAttackToMove();
    void switchAttackToStand();
    void switchAttackToDie();

    bool canSwitchDieToDie();
    bool canSwitchDieToMove();
    bool canSwitchDieToStand();
    bool canSwitchDieToAttack();
    void switchDieToDie();
    void switchDieToMove();
    void switchDieToStand();
    void switchDieToAttack();

    void onMoveTo();
    void onStand();
    void onDie();
    void onDieAnimationEnd();

    void onAttack();
    void onAttackAnimationEnd();

    RepeatForever* createAnimateWidthPList(const string& plist, float animateDelayPerUnit, NpcStatus animateType);

    FaceDirection getFaceToDirection(const Vec2& moveToPosition);
    float getMoveToDuration(const Vec2& moveToPosition);
    float _perSecondMoveSpeedByPixel = 100.0f;

    list<Vec2> getPathListTo(const Vec2& inMapEndPosition);
    list<Vec2> _gotoTargetPositionPathList;

    unordered_map<FaceDirection, RepeatForever*> _moveAnimateMap;
    unordered_map<FaceDirection, RepeatForever*> _standAnimateMap;
    unordered_map<FaceDirection, RepeatForever*> _attackAnimateMap;
    Sprite* _shadowSprite = nullptr;
    Size _dieAnimationFrameSize;

    RepeatForever* _dieAnimate = nullptr;
    FaceDirection _faceDirection = FaceDirection::Invalid;

    int _maxAlertRadius = 0;
    bool _isReadyToMove = false;

    GameWorldCallBackFunctionsManager* _gameWorld = nullptr;

    Vec2 _inMapPosition;
    float _reinforceRadius = 0.0f;

    float _maxAttackRangeWhenCollision = 0.0f;

    MopUpCommand _mopUpCommand; // 扫荡指令，即A过去的功能
};