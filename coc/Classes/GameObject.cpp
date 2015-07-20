#include "Base.h"
#include "GameObject.h"
#include "Npc.h"
#include "Building.h"
#include "GameSetting.h"
#include "ForceManager.h"
#include "GameConfigManager.h"

static int g_uniqueID = 0;

const float MAX_SHOW_HP_BAR_TIME_LIMIT_AFTER_BEING_ATTACKED = 3.0f;

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

    _selectedTips = Sprite::create();
    _selectedTips->setVisible(false);
    addChild(_selectedTips, -1);

    TTFConfig config("arial.ttf", 24);
    _teamIDLabel = Label::createWithTTF(config, "");
    addChild(_teamIDLabel, -1);

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

    _selectedTips->setVisible(isSelect);
    _teamIDLabel->setVisible(isSelect);
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
    _showHPBarTotalTimeAfterBeingAttacked = 0.0f;
    showHPBar();

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

int GameObject::getEnemyUniqueID()
{
    return _enemyUniqueID;
}

void GameObject::update(float delta)
{
    if (g_setting.allowDebugDraw)
    {
        debugDraw();
    }

    updateHP();

    _forbidEnemyApproachTime -= delta;

    _showHPBarTotalTimeAfterBeingAttacked += delta;
    if (!isSelected() &&
        _showHPBarTotalTimeAfterBeingAttacked >= MAX_SHOW_HP_BAR_TIME_LIMIT_AFTER_BEING_ATTACKED)
    {
        hideHPBar();
    }
}

void GameObject::clearDebugDraw()
{
    _debugDrawNode->clear();
    _debugDrawNode->setVisible(false);
}

float GameObject::getCollisionRadius()
{
    return _collisionRadius;
}

DamageType GameObject::getDamageType()
{
    return _damageType;
}

float GameObject::getAoeDamageRadius()
{
    return _aoeDamageRadius;
}

GameObject* GameObjectFactory::create(GameObjectType gameObjectType, ForceType forceType, const string& jobName, const Vec2& position, int level)
{
    GameObject* gameObject = nullptr;

    level = std::max(level, 0);
    level = std::min(level, MAX_LEVEL);

    switch (gameObjectType)
    {
        case GameObjectType::DefenceInBuildingNpc:
        case GameObjectType::Npc:
        {
            g_uniqueID++;
            gameObject = Npc::create(forceType, gameObjectType, jobName, position, g_uniqueID, level);
        }
        break;
        case GameObjectType::Building:
        {
            g_uniqueID++;
            gameObject = Building::create(forceType, jobName, position, g_uniqueID, level);
        }
        break;
    default:
        break;
    }

    return gameObject;
}

int GameObject::getTeamID()
{
    return _teamID;
}

void GameObject::setTeamID(int teamID)
{
    if (teamID == TEAM_INVALID_ID)
    {
        _teamIDLabel->setString("");
    }
    else
    {
        _teamID = teamID;
        _teamIDLabel->setString(StringUtils::format("%d", teamID));
    }
}

const string& GameObject::getTemplateName()
{
    return _templateName;
}

bool GameObject::canEnemyApproach()
{
    return _forbidEnemyApproachTime <= 0.0f;
}

void GameObject::launchForbidEnemyApproachTimer()
{
    _forbidEnemyApproachTime = FORBID_ENEMY_APPROACH_TIME_INTERVAL;
}

void GameObject::onAddEnemyTechnologyPoint()
{
    Color4F textColor;

    auto forceManager = ForceManager::getInstance();
    if (_forceType == ForceType::Player)
    {
        forceManager->addTechnologyPoint(ForceType::AI, _technologyPointForEnemy);
        textColor = g_setting.aiForceColor;
    }
    else
    {
        forceManager->addTechnologyPoint(ForceType::Player, _technologyPointForEnemy);
        textColor = g_setting.playerForceColor;
    }

    TTFConfig config(g_setting.fontName.c_str(), 48);
    auto technologyPointForEnemyLabel = Label::createWithTTF(config, StringUtils::format("+%d", _technologyPointForEnemy));
    technologyPointForEnemyLabel->setColor(Color3B(textColor));
    technologyPointForEnemyLabel->setPosition(Vec2(0.0f, _contentSize.width / 2.0f));
    addChild(technologyPointForEnemyLabel, 1000);

    auto spawnAction = Spawn::create(MoveBy::create(1.0f, Vec2(0.0f, 150.0f)), FadeTo::create(1.0f, 0), nullptr);
    technologyPointForEnemyLabel->runAction(spawnAction);
}

void GameObject::upgrade()
{
    _level++;
    _level = std::max(_level, 0);
    _level = std::min(_level, MAX_LEVEL);

    updatePropertyBy(_level);
}

void GameObject::updatePropertyBy(int level)
{
    auto gameObjectLevelConfig = GameConfigManager::getInstance()->getGameObjectLevelConfig(_templateName, level);
    if (gameObjectLevelConfig)
    {
        _level = level;
        _attackPower = gameObjectLevelConfig->attackPower;
        _maxHp = _hp = gameObjectLevelConfig->hp;
        updateLevelRepresentTexture(gameObjectLevelConfig->levelRepresentTextureName);
    }
}
