#pragma once

enum class GameObjectType
{
    Invalid,

    Npc,
    Building,
};

class GameObject : public Sprite
{
public:
    virtual ~GameObject();
    int getUniqueID();
protected:
    int _hp = 100;
    int _maxHp = 100;

    int _level = 1;
    int _maxLevel = 3;

    int _uniqueID = 0;

    GameObjectType _type = GameObjectType::Invalid;
};

class GameObjectFactory
{
public:
    static GameObject* create(GameObjectType type, const string& jobName, const Vec2& position);
};