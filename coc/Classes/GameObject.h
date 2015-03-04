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

    static GameObject* create(GameObjectType type, const string& jobName, const Vec2& position);
protected:
    int _hp = 100;
    int _maxHp = 100;
    int _level = 1;
    int _maxLevel = 3;
    GameObjectType _type = GameObjectType::Invalid;
};