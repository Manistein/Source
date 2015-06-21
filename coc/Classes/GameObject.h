#pragma once

enum class GameObjectType
{
    Invalid,

    Npc,
    DefenceInBuildingNpc,
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
    Magic,
};

enum class DamageType
{
    Invalid,

    AreaOfEffect,
    Normal,
};

const int ENEMY_UNIQUE_ID_INVALID = 0;
const int GAME_OBJECT_UNIQUE_ID_INVALID = 0; 
const string HP_BAR_BACKGROUND_TEXTURE_NAME = "HPBarBackground.png";
const int TEAM_INVALID_ID = 0;

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
    float getExtraEnemyAttackRadius();
    DamageType getDamageType();
    float getAoeDamageRadius();

    void showHPBar();
    void hideHPBar();
    virtual void costHP(int costHPAmount);
    void addHP(int addHPAmount);
    void updateHP();

    virtual void setEnemyUniqueID(int uniqueID);
    virtual int getEnemyUniqueID();

    void update(float delta) override;
    virtual void clearDebugDraw();

    float getCollisionRadius();

    int getTeamID();
    void setTeamID(int teamID);
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
    float _extraEnemyAttackRadius = 0.0f;   // µÐ·½GameObject¹¥»÷·¶Î§¼Ó³É
    BulletType _bulletType = BulletType::Invalid;
    DamageType _damageType = DamageType::Invalid;
    float _aoeDamageRadius = 0.0f;

    int _level = 0;
    int _maxLevel = 0;

    int _enemyUniqueID = ENEMY_UNIQUE_ID_INVALID;
    int _uniqueID = 0;

    bool _isSelected = false;

    GameObjectType _gameObjectType = GameObjectType::Invalid;
    ForceType _forceType = ForceType::Invalid;

    DrawNode* _debugDrawNode = nullptr;
    Sprite* _selectedTips = nullptr;
    Label* _teamIDLabel = nullptr;
    int _teamID = TEAM_INVALID_ID;

    float _showHPBarTotalTimeAfterBeingAttacked = 0.0f;

    float _collisionRadius = 0.0f;
};

class GameObjectFactory
{
public:
    static GameObject* create(GameObjectType gameObjectType, ForceType forceType, const string& jobName, const Vec2& position);
};