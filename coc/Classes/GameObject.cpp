#include "Base.h"
#include "GameObject.h"
#include "Npc.h"
#include "Building.h"
#include "GameSetting.h"

static int g_uniqueID = 0;

const string PLAYER_HP_BAR_TEXTURE_NAME = "PlayerHPBar.png";
const string AI_HP_BAR_TEXTURE_NAME = "AIHPBar.png";

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

    if (_forceType == ForceType::Player)
    {
        _hpBar = ui::LoadingBar::create(PLAYER_HP_BAR_TEXTURE_NAME);
    }
    else
    {
        _hpBar = ui::LoadingBar::create(AI_HP_BAR_TEXTURE_NAME);
    }

    _hpBar->setAnchorPoint(Vec2::ZERO);
    _hpBar->setPercent(100.0f);

    auto hpBarBackground = Sprite::create(HP_BAR_BACKGROUND_TEXTURE_NAME);
    hpBarBackground->setCascadeOpacityEnabled(true);
    hpBarBackground->setScale(0.5f);
    hpBarBackground->addChild(_hpBar);
    hpBarBackground->setVisible(false);

    addChild(hpBarBackground);

    _debugDrawNode = DrawNode::create();
    _debugDrawNode->setCascadeOpacityEnabled(true);
    addChild(_debugDrawNode, 1);

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
    //setGlobalZOrder(MAX_GAME_OBJECT_COUNT + newZ);
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

int GameObject::getAttackPower()
{
    return _attackPower;
}

float GameObject::getExtraEnemyAttackRadius()
{
    return _extraEnemyAttackRadius;
}

void GameObject::costHP(int costHPAmount)
{
    _hp = std::max(0, _hp - costHPAmount);

    if (_hp <= 0)
    {
        hideHPBar();
        onPrepareToRemove();
    }
}

void GameObject::addHP(int addHPAmount)
{
    _hp = std::min(_hp + addHPAmount, _maxHp);
}

void GameObject::updateHP()
{
    float hpPercent = (float)_hp / (float)_maxHp;
    _hpBar->setPercent(hpPercent * 100.0f);
}

void GameObject::setEnemyUniqueID(int uniqueID)
{
    _enemyUniqueID = uniqueID;
}

void GameObject::update(float delta)
{
    if (g_setting.allowDebugDraw)
    {
        debugDraw();
    }

    updateHP();
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
        {
            g_uniqueID++;
            gameObject = Building::create(forceType, jobName, position, g_uniqueID);
        }
        break;
    default:
        break;
    }

    return gameObject;
}
