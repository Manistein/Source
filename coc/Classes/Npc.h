#pragma once

enum class NpcStatus
{
    Invalid,

    Move,
    Stand,
    Attack,
    Die
};

class Npc : public GameObject
{
public:
    ~Npc();

    static Npc* create(const string& jobName, const Vec2& position, int uniqueID);
private:
    bool init(const string& jobName, const Vec2& position, int uniqueID);

    RepeatForever* createAnimateWidthPList(const string& plist);

    RepeatForever* _moveToEastAnimate = nullptr;
    RepeatForever* _moveToNorthEastAnimate = nullptr;
    RepeatForever* _moveToNorthWestAnimate = nullptr;
    RepeatForever* _moveToSouthEastAnimate = nullptr;
    RepeatForever* _moveToSouthWestAnimate = nullptr;
    RepeatForever* _moveToWestAnimate = nullptr;
    RepeatForever* _dieAnimate = nullptr;
    RepeatForever* _standAnimate = nullptr;

    NpcStatus _oldStatus = NpcStatus::Stand;
    NpcStatus _newStatus = NpcStatus::Stand;
};