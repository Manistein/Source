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

const float CD_TIME_INTERVAL = 0.5f;

class Npc : public GameObject
{
public:
    ~Npc();

    static Npc* create(ForceType forceType, const string& jobName, const Vec2& position, int uniqueID);

    void moveTo(const Vec2& targetPosition);
private:
    bool init(ForceType forceType, const string& jobName, const Vec2& position, int uniqueID);
    void clear();

    void initAnimates(const string& jobName);
    void initSwitchStatusFunctions();
    void initShadow();
    void initHPBar();
    void initBattleData(const string& jobName);

    void update(float delta) override;
    void updateStatus(NpcStatus newStatus);

    void runAI(float delta);
    bool isEnemyInAttackRange(GameObject* enemy);
    bool isEnemyInAlertRange(GameObject* enemy);
    float getDistanceFrom(GameObject* enemy);
    float _cdTimeDelta = CD_TIME_INTERVAL;

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
    void onAttack();

    RepeatForever* createAnimateWidthPList(const string& plist, float animateDelayPerUnit);

    FaceDirection getFaceToDirection(const Vec2& moveToPosition);
    float getMoveToDuration(const Vec2& moveToPosition);
    float _perSecondMoveSpeedByPixel = 100.0f;
    Vec2 _moveToPosition;

    unordered_map<FaceDirection, RepeatForever*> _moveAnimateMap;
    unordered_map<FaceDirection, RepeatForever*> _standAnimateMap;

    RepeatForever* _dieAnimate = nullptr;
    FaceDirection _faceDirection = FaceDirection::Invalid;

    int _maxAlertRadius = 0;
};