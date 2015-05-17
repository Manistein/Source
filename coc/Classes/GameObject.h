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

enum class BulletType
{
    Invalid,

    Arrow,
};

enum class DamageType
{
    Invalid,

    AreaOfEffect,
    Normal,
};

const int ENEMY_UNIQUE_ID_INVALID = 0;
const int GAME_OBJECT_UNIQUE_ID_INVALID = 0; 

class GameObject : public Sprite
{
public:
    virtual ~GameObject();
    int getUniqueID();
    void depthSort(const Size& tileSize);

    void setSelected(bool isSelect);
    bool isSelected();
    virtual bool isReadyToRemove() = 0;

    GameObjectType getGameObjectType();
    ForceType getForceType();
    int getAttackPower();

    void showHPBar();
    void hideHPBar();
    void costHP(int costHPAmount);
    void addHP(int addHPAmount);
    void updateHP();

    void setEnemyUniqueID(int uniqueID);

    void update(float delta) override;
    virtual void clearDebugDraw() = 0;
protected:
    GameObject();
    bool init() override;
    virtual void debugDraw() = 0;
    virtual void onPrepareToRemove() = 0;

    int _hp = 0;
    int _maxHp = 0;
    ui::LoadingBar* _hpBar = nullptr;

    int _attackPower = 0;
    int _maxAttackRadius = 0;
    BulletType _bulletType = BulletType::Invalid;
    DamageType _damageType = DamageType::Invalid;

    int _level = 0;
    int _maxLevel = 0;

    int _enemyUniqueID = ENEMY_UNIQUE_ID_INVALID;
    int _uniqueID = 0;

    bool _isSelected = false;

    GameObjectType _gameObjectType = GameObjectType::Invalid;
    ForceType _forceType = ForceType::Invalid;

    DrawNode* _debugDrawNode = nullptr;
};

class GameObjectFactory
{
public:
    static GameObject* create(GameObjectType gameObjectType, ForceType forceType, const string& jobName, const Vec2& position);
};