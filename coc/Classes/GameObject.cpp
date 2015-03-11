#include "Base.h"
#include "GameObject.h"
#include "Npc.h"

static int g_uniqueID = 0;

GameObject::~GameObject()
{

}

int GameObject::getUniqueID()
{
    return _uniqueID;
}

void GameObject::depthSort(const Size& tileSize)
{
    auto position = getPosition();
    position = CC_POINT_POINTS_TO_PIXELS(position);
    float newZ = -(position.y + tileSize.height / 3.0f) / (tileSize.height / 2.0f);
    setPositionZ(newZ);
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
