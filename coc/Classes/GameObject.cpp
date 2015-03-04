#include "Base.h"
#include "GameObject.h"

GameObject::~GameObject()
{

}

GameObject* GameObject::create(GameObjectType type, const string& jobName, const Vec2& position)
{
    GameObject* gameObject = nullptr;

    switch (type)
    {
    case GameObjectType::Npc:
        break;
    case GameObjectType::Building:
        break;
    default:
        break;
    }

    return gameObject;
}
