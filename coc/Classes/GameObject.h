#pragma once

enum class GameObjectType
{
    Invalid,

    Npc,
    Building,
};

enum class ForceType
{
    Invalid,

    Player,
    AI,
};

class GameObject : public Sprite
{
public:
    virtual ~GameObject();
    int getUniqueID();
    void depthSort(const Size& tileSize);

    void showHPBar();
    void hideHPBar();
protected:
    GameObject();
    bool init() override;

    int _hp = 100;
    int _maxHp = 100;
    ui::LoadingBar* _hpBar = nullptr;

    int _level = 1;
    int _maxLevel = 3;

    int _uniqueID = 0;

    GameObjectType _gameObjectType = GameObjectType::Invalid;
    ForceType _forceType = ForceType::Invalid;
};

class GameObjectFactory
{
public:
    static GameObject* create(GameObjectType type, const string& jobName, const Vec2& position);
};