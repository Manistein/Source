#include "Base.h"
#include "GameObject.h"
#include "Npc.h"

static int g_uniqueID = 0;

const string HP_BAR_BACKGROUND_TEXTURE_NAME = "HPBarBackground.png";
const string PLAYER_HP_BAR_TEXTURE_NAME = "PlayerHPBar.png";

GameObject::~GameObject()
{

}

GameObject::GameObject()
{

}

bool GameObject::init()
{
    if (!Sprite::init())
    {
        return false;
    }

    _hpBar = ui::LoadingBar::create(PLAYER_HP_BAR_TEXTURE_NAME);
    _hpBar->setAnchorPoint(Vec2::ZERO);
    _hpBar->setPercent(100.0f);

    auto hpBarBackground = Sprite::create(HP_BAR_BACKGROUND_TEXTURE_NAME);
    hpBarBackground->setCascadeOpacityEnabled(true);
    hpBarBackground->setScale(0.5f);
    hpBarBackground->addChild(_hpBar);
    hpBarBackground->setVisible(false);

    addChild(hpBarBackground);

    return true;
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
    setGlobalZOrder(MAX_GAME_OBJECT_COUNT + newZ);
}

void GameObject::showHPBar()
{
    auto hpBarBackground = _hpBar->getParent();
    hpBarBackground->setVisible(true);
}

void GameObject::hideHPBar()
{
    auto hpBarBackground = _hpBar->getParent();
    hpBarBackground->setVisible(false);
}

void GameObject::setSelected(bool isSelect)
{
    if (isSelect)
    {
        showHPBar();
    }
    else
    {
        hideHPBar();
    }

    _isSelected = isSelect;
}

bool GameObject::isSelected()
{
    return _isSelected;
}

GameObjectType GameObject::getGameObjectType()
{
    return _gameObjectType;
}

ForceType GameObject::getForceType()
{
    return _forceType;
}

GameObject* GameObjectFactory::create(GameObjectType gameObjectType, ForceType forceType, const string& jobName, const Vec2& position)
{
    GameObject* gameObject = nullptr;

    switch (gameObjectType)
    {
        case GameObjectType::Npc:
        {
            g_uniqueID++;
            gameObject = Npc::create(forceType, jobName, position, g_uniqueID);
        }
        break;
    case GameObjectType::Building:
        break;
    default:
        break;
    }

    return gameObject;
}
