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
protected:
    int _hp = 0;
    GameObjectType _type = GameObjectType::Invalid;
};