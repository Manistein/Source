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

class Npc : public GameObject
{
public:
    ~Npc();

    static Npc* create(const string& jobName, const Vec2& position, int uniqueID);

    void moveTo(const Vec2& targetPosition);
private:
    bool init(const string& jobName, const Vec2& position, int uniqueID);
    void clear();

    void initAnimates(const string& jobName);
    void initSwitchStatusFunctions();

    void updateStatus(NpcStatus newStatus);

    struct SwitchStatusFunction 
    {
        std::function<void()> switchFunction;
        std::function<bool()> canSwitch;
    };
    SwitchStatusFunction _switchStatusFunctions[(int)NpcStatus::Total][(int)NpcStatus::Total];

    void registerSwitchStatusFunction(SwitchStatusFunction& switchStatusFunctions, std::function<bool()> canSwitch, std::function<void()> switchFunction);

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

    void onMoveToEvent();
    void onStandEvent();

    RepeatForever* createAnimateWidthPList(const string& plist);

    FaceDirection getFaceToDirection(const Vec2& moveToPosition);
    float getMoveToDuration(const Vec2& moveToPosition);
    float _perSecondMoveSpeedByPixel = 100.0f;
    Vec2 _moveToPosition;

    unordered_map<FaceDirection, RepeatForever*> _moveAnimateMap;
    unordered_map<FaceDirection, RepeatForever*> _standAnimateMap;

    RepeatForever* _dieAnimate = nullptr;

    NpcStatus _oldStatus = NpcStatus::Stand;
    FaceDirection _faceDirection = FaceDirection::Invalid;
};