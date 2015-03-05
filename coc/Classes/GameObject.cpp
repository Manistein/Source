#include "Base.h"
#include "GameObject.h"
#include "Npc.h"

static int g_uniqueID = 0;

GameObject::~GameObject()
{

}

GameObject* GameObjectFactory::create(GameObjectType type, const string& jobName, const Vec2& position)
{
    GameObject* gameObject = nullptr;

    switch (type)
    {
        case GameObjectType::Npc:
        {
            g_uniqueID++;
            gameObject = Npc::create(jobName, position, g_uniqueID);
        }
        break;
    case GameObjectType::Building:
        break;
    default:
        break;
    }

    return gameObject;
}
