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

    static GameObject* create(GameObjectType type, const string& jobName);
protected:
    int _hp = 0;
    int _maxHp = 0;
    GameObjectType _type = GameObjectType::Invalid;
};