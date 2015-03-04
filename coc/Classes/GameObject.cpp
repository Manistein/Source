#include "Base.h"
#include "GameObject.h"

GameObject::~GameObject()
{

}

GameObject* GameObject::create(GameObjectType type, const string& jobName)
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
