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

enum class AttackType
{
    Invalid,

    ShortRange,
    LongRange,
};

enum class DamageType
{
    Invalid,

    AreaOfEffect,
    Normal,
};

class GameObject : public Sprite
{
public:
    virtual ~GameObject();
    int getUniqueID();
    void depthSort(const Size& tileSize);

    void showHPBar();
    void hideHPBar();

    void setSelected(bool isSelect);
    bool isSelected();

    GameObjectType getGameObjectType();
    ForceType getForceType();

    void updateHP();
protected:
    GameObject();
    bool init() override;

    int _hp = 100;
    int _maxHp = 100;
    ui::LoadingBar* _hpBar = nullptr;

    int _attackPower = 10;
    int _maxAttackRadius = 10;
    int _perSecondAttackCount = 5;
    AttackType _attackType = AttackType::Invalid;
    DamageType _damageType = DamageType::Invalid;

    int _level = 1;
    int _maxLevel = 3;

    int _enemyUniqueID = 0;
    int _uniqueID = 0;

    bool _isSelected = false;

    GameObjectType _gameObjectType = GameObjectType::Invalid;
    ForceType _forceType = ForceType::Invalid;
};

class GameObjectFactory
{
public:
    static GameObject* create(GameObjectType gameObjectType, ForceType forceType, const string& jobName, const Vec2& position);
};