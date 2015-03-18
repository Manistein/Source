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

const int ENEMY_UNIQUE_ID_INVALID = 0;

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
    void setEnemyUniqueID(int uniqueID);
protected:
    GameObject();
    bool init() override;

    int _hp = 0;
    int _maxHp = 0;
    ui::LoadingBar* _hpBar = nullptr;

    int _attackPower = 0;
    int _maxAttackRadius = 0;
    int _perSecondAttackCount = 0;
    AttackType _attackType = AttackType::Invalid;
    DamageType _damageType = DamageType::Invalid;

    int _level = 0;
    int _maxLevel = 0;

    int _enemyUniqueID = ENEMY_UNIQUE_ID_INVALID;
    bool _isEnemyChange = false;
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