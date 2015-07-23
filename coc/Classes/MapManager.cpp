#include "Base.h"
#include "MapManager.h"
#include "AutoFindPathHelper.h"

const int COMMAND_TIPS_RUN_ACTION_TIMES = 5;

static std::map<TileMapLayerType, std::string> s_tileMapLayerTypeToString = {
    { TileMapLayerType::BackgroundLayer, "backgroundLayer" },
    { TileMapLayerType::GameObjcetLayer, "gameObjectLayer" },
};


MapManager::~MapManager()
{
    Director::getInstance()->setProjection(Director::Projection::DEFAULT);
    Director::getInstance()->setDepthTest(false);

    for (int rowIndex = 0; rowIndex < (int)_tileNodeTable.size(); rowIndex ++)
    {
        for (int columnIndex = 0; columnIndex < (int)_tileNodeTable[rowIndex].size(); columnIndex ++)
        {
            CC_SAFE_DELETE(_tileNodeTable[rowIndex][columnIndex]);
        }
    }
    _tileNodeTable.clear();
}

bool MapManager::init(Node* parentNode, const std::string& titleMapFileName)
{
    auto glView = Director::getInstance()->getOpenGLView();
    auto windowHandle = glView->getWin32Window();
    GetClientRect(windowHandle, &_clientRect);

    _tileMap = cocos2d::experimental::TMXTiledMap::create(titleMapFileName);
    _tileMap->setScale(_mapScale);
    _tileMap->setPosition(Vec2(-500.0f, -500.0f));
    parentNode->addChild(_tileMap);

    Director::getInstance()->setProjection(Director::Projection::_2D);
    Director::getInstance()->setDepthTest(true);

    glAlphaFunc(GL_GREATER, (GLclampf)0.1);
    glEnable(GL_ALPHA_TEST);

    auto visibleSize = Director::getInstance()->getVisibleSize();
    POINT cursorInClientPoint;
    ::GetCursorPos(&cursorInClientPoint);
    ::ScreenToClient(windowHandle, &cursorInClientPoint);
    _cursorPoint.x = cursorInClientPoint.x;
    _cursorPoint.y = visibleSize.height - cursorInClientPoint.y;

    initTileNodeTable();
    AutoFindPathHelper::initTileNodeTable(_tileNodeTable);

    //resolveMapShakeWhenMove();

    initMopUpCommandTips();
    initMoveCommandTips();

    return true;
}

void MapManager::updateMapPosition()
{
    auto titleMapPosition = _tileMap->getPosition();
    auto titleMapSize = _tileMap->getContentSize() * _tileMap->getScale();

    if (_cursorPoint.x <= _clientRect.left + MAP_BORDER_MARGIN)
    {
        titleMapPosition.x += MAP_MOVE_SPEED;
    }

    if (_cursorPoint.y <= _clientRect.top + MAP_BORDER_MARGIN)
    {
        titleMapPosition.y += MAP_MOVE_SPEED;
    }

    if (_cursorPoint.x >= _clientRect.right - MAP_BORDER_MARGIN)
    {
        titleMapPosition.x -= MAP_MOVE_SPEED;
    }

    if (_cursorPoint.y >= _clientRect.bottom - MAP_BORDER_MARGIN)
    {
        titleMapPosition.y -= MAP_MOVE_SPEED;
    }

    if (titleMapPosition.x > 0.0f)
    {
        titleMapPosition.x = 0.0f;
    }

    auto clientWidth = _clientRect.right - _clientRect.left;
    auto clientHeight = _clientRect.bottom - _clientRect.top;
    auto mapMinXPosition = clientWidth - titleMapSize.width;
    auto mapMinYPosition = clientHeight - titleMapSize.height;
    titleMapPosition.x = std::max(titleMapPosition.x, mapMinXPosition);
    titleMapPosition.y = std::max(titleMapPosition.y, mapMinYPosition);
    titleMapPosition.x = std::min(titleMapPosition.x, 0.0f);
    titleMapPosition.y = std::min(titleMapPosition.y, MAP_BOTTOM_MARGIN);

    _tileMap->setPosition(titleMapPosition);
}

void MapManager::updateMapScale(Event* event)
{
    auto lastMapScale = _mapScale;

    auto mouseEvent = static_cast<EventMouse*>(event);
    if (mouseEvent->getScrollY() == 1)
    {
        _mapScale = std::max(_mapScale - 0.1f, MAP_MIN_SCALE);
    }
    else
    {
        _mapScale = std::min(_mapScale + 0.1f, MAP_MAX_SCALE);
    }

    auto tileMapContentSize = _tileMap->getContentSize();
    auto visibleSize = Director::getInstance()->getVisibleSize();
    if (tileMapContentSize.width * _mapScale < visibleSize.width || tileMapContentSize.height * _mapScale < visibleSize.height)
    {
        return;
    }

    auto cursorPointInMap = _tileMap->convertToNodeSpace(_cursorPoint);
    auto cursorScalePointInMap = cursorPointInMap * _mapScale;
    auto moveDelta = cursorPointInMap * lastMapScale - cursorScalePointInMap;

    auto tileMapPosition = _tileMap->getPosition();
    _tileMap->setPosition(tileMapPosition + moveDelta);
    _tileMap->setScale(_mapScale);
}

void MapManager::syncCursorPoint(const Vec2& cursorPoint)
{
    _cursorPoint = cursorPoint;
}

bool MapManager::isInObstacleTile(const Vec2& inMapPosition)
{
    bool result = false;

    auto tileSubscript = getTileSubscript(inMapPosition);
    int columnIndex = (int)tileSubscript.x;
    int rowIndex = (int)tileSubscript.y;

    auto tileNode = getTileNodeAt(columnIndex, rowIndex);
    if (tileNode->gid == OBSTACLE_ID)
    {
        result = true;
    }

    return result;
}

void MapManager::resolveMapShakeWhenMove()
{
    auto& children = _tileMap->getChildren();
    for (auto& child : children)
    {
        auto tile = static_cast<Sprite*>(child);
        auto tileTexture = tile->getTexture();
        if (tileTexture)
        {
            tileTexture->setAntiAliasTexParameters();
        }
    }
}

void MapManager::setTileMapPosition(const Vec2& worldPosition)
{
    _tileMap->setPosition(worldPosition);
}

Size MapManager::getTileSize()
{
    return _tileMap->getTileSize();
}

Size MapManager::getMapSize()
{
    return _tileMap->getMapSize();
}

Point MapManager::getTileSubscript(const Vec2& inMapPosition)
{
    Point tileSubscript;

    auto mapSize = _tileMap->getMapSize();
    auto tileSize = _tileMap->getTileSize();
    //auto inMapPosition = _tileMap->convertToNodeSpace(_cursorPoint);
    tileSubscript.x = (mapSize.height - (inMapPosition.y / tileSize.height)) + ((inMapPosition.x / tileSize.width) - mapSize.width / 2);
    tileSubscript.y = (mapSize.height - (inMapPosition.y / tileSize.height)) - ((inMapPosition.x / tileSize.width) - mapSize.width / 2);

    tileSubscript.x = std::max(0.0f, tileSubscript.x);
    tileSubscript.x = std::min(mapSize.width - 1.0f, tileSubscript.x);
    tileSubscript.y = std::max(0.0f, tileSubscript.y);
    tileSubscript.y = std::min(mapSize.height - 1.0f, tileSubscript.y);

    tileSubscript.x = std::floor(tileSubscript.x);
    tileSubscript.y = std::floor(tileSubscript.y);

    return tileSubscript;
}

MapDebugInfo MapManager::getMapDebugInfo(TileMapLayerType tileMapLayerType)
{
    MapDebugInfo tileDebugInfo;

    auto currentTileMapLayer = _tileMap->getLayer(s_tileMapLayerTypeToString[tileMapLayerType]);
    auto tileSubscript = getTileSubscript(_tileMap->convertToNodeSpace(_cursorPoint));

    tileDebugInfo.gid = currentTileMapLayer->getTileGIDAt(tileSubscript);
    tileDebugInfo.tileMapLayerName = s_tileMapLayerTypeToString[tileMapLayerType];
    tileDebugInfo.cursorPoint = _cursorPoint;
    tileDebugInfo.tileSubscript = tileSubscript;

    return tileDebugInfo;
}

float MapManager::getMapScale()
{
    return _tileMap->getScale();
}

const Vec2& MapManager::getMapPosition()
{
    return _tileMap->getPosition();
}

const Vector<TMXObjectGroup*>&  MapManager::getEditedGameObjectGroup()
{
    return _tileMap->getObjectGroups();
}

cocos2d::Vec2 MapManager::convertCursorPositionToTileMapSpace()
{
    return _tileMap->convertToNodeSpace(_cursorPoint);
}

cocos2d::Vec2 MapManager::convertToTileMapSpace(const Vec2& worldPosition)
{
    return _tileMap->convertToNodeSpace(worldPosition);
}

void MapManager::addChildInGameObjectLayer(Node* gameObject, int zOrder/* = 1*/)
{
    auto gameObjectLayer = _tileMap->getLayer("gameObjectLayer");
    gameObjectLayer->addChild(gameObject, zOrder);
}

void MapManager::initTileNodeTable()
{
    auto mapSize = _tileMap->getMapSize();
    _tileNodeTable.resize((int)mapSize.width);

    auto tileSize = _tileMap->getTileSize();

    auto gameObjectLayer = _tileMap->getLayer(s_tileMapLayerTypeToString[TileMapLayerType::GameObjcetLayer]);

    for (int columnIndex = 0; columnIndex < (int)mapSize.width; columnIndex ++)
    {
        _tileNodeTable[columnIndex].resize((int)mapSize.height);
        for (int rowIndex = 0; rowIndex < (int)mapSize.height; rowIndex ++)
        {
            _tileNodeTable[columnIndex][rowIndex] = new TileNode;

            _tileNodeTable[columnIndex][rowIndex]->gid = gameObjectLayer->getTileGIDAt(Vec2(columnIndex, rowIndex));

            Vec2 positionInTileMap;
            positionInTileMap.x = (((float)columnIndex - (float)rowIndex) / 2.0f + mapSize.width / 2.0f) * tileSize.width;
            positionInTileMap.y = (mapSize.height - ((float)columnIndex + (float)rowIndex) / 2.0f) *  tileSize.height;
            _tileNodeTable[columnIndex][rowIndex]->leftTopPosition = positionInTileMap;

            _tileNodeTable[columnIndex][rowIndex]->rowIndex = rowIndex;
            _tileNodeTable[columnIndex][rowIndex]->columnIndex = columnIndex;

            _tileNodeTable[columnIndex][rowIndex]->parent = nullptr;

            _tileNodeTable[columnIndex][rowIndex]->isVisit = false;
        }
    }
}

TileNode* MapManager::getTileNodeAt(int columnIndex, int rowIndex)
{
    return _tileNodeTable[columnIndex][rowIndex];
}

void MapManager::initMopUpCommandTips()
{
    _mopUpCommandTips = Sprite::create("mopUpCommandTips.png");
    _mopUpCommandTips->setVisible(false);

    auto gameObjectLayer = _tileMap->getLayer(s_tileMapLayerTypeToString[TileMapLayerType::GameObjcetLayer]);
    gameObjectLayer->addChild(_mopUpCommandTips, 1);
}

void MapManager::initMoveCommandTips()
{
    _moveCommandTips = Sprite::create("moveCommandTips.png");
    _moveCommandTips->setVisible(false);

    auto gameObjectLayer = _tileMap->getLayer(s_tileMapLayerTypeToString[TileMapLayerType::GameObjcetLayer]);
    gameObjectLayer->addChild(_moveCommandTips, 1);
}

void MapManager::showMopUpCommandTips()
{
    _mopUpCommandTips->stopAllActions();
    auto inMapPosition = convertCursorPositionToTileMapSpace();
    _mopUpCommandTips->setPosition(inMapPosition);

    _mopUpCommandTips->setVisible(true);

    auto repeatAction = Repeat::create(
            Sequence::create(
            ScaleTo::create(0.2f, 0.7f),
            ScaleTo::create(0.2f, 1.0f),
            nullptr),
         COMMAND_TIPS_RUN_ACTION_TIMES);

    auto callBackFunction = CallFunc::create(CC_CALLBACK_0(Sprite::setVisible, _mopUpCommandTips, false));

    auto sequenceAction = Sequence::create(
        repeatAction,
        callBackFunction,
        nullptr
        );

    _mopUpCommandTips->runAction(sequenceAction);
}

void MapManager::hideMopUpCommandTips()
{
    _mopUpCommandTips->stopAllActions();
    _mopUpCommandTips->setVisible(false);
}

void MapManager::showMoveCommandTips()
{
    _moveCommandTips->stopAllActions();
    auto inMapPosition = convertCursorPositionToTileMapSpace();
    _moveCommandTips->setPosition(inMapPosition);

    _moveCommandTips->setVisible(true);

    auto repeatAction = Repeat::create(
        Sequence::create(
        MoveBy::create(0.2f, Vec2(0.0f, 10.0f)),
        MoveBy::create(0.2f, Vec2(0.0f, -10.0f)),
        nullptr),
        COMMAND_TIPS_RUN_ACTION_TIMES);

    auto callBackFunction = CallFunc::create(CC_CALLBACK_0(Sprite::setVisible, _moveCommandTips, false));

    auto sequenceAction = Sequence::create(
        repeatAction,
        callBackFunction,
        nullptr
        );

    _moveCommandTips->runAction(sequenceAction);
}

void MapManager::hideMoveCommandTips()
{
    _moveCommandTips->stopAllActions();
    _moveCommandTips->setVisible(false);
}