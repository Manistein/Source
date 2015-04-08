#include "Base.h"
#include "MapManager.h"


static std::map<TileMapLayerType, std::string> s_tileMapLayerTypeToString = {
    { TileMapLayerType::BackgroundLayer, "backgroundLayer" },
    { TileMapLayerType::GameObjcetLayer, "gameObjectLayer" },
};


MapManager::~MapManager()
{
    Director::getInstance()->setProjection(Director::Projection::DEFAULT);
    Director::getInstance()->setDepthTest(false);
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

    auto visibleSize = Director::getInstance()->getVisibleSize();
    POINT cursorInClientPoint;
    ::GetCursorPos(&cursorInClientPoint);
    ::ScreenToClient(windowHandle, &cursorInClientPoint);
    _cursorPoint.x = cursorInClientPoint.x;
    _cursorPoint.y = visibleSize.height - cursorInClientPoint.y;

    initTileMapInfoTable();

    //resolveMapShakeWhenMove();

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
    titleMapPosition.y = std::min(titleMapPosition.y, 0.0f);

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

vector<Vec2> MapManager::getNpcMoveEndPositionListBy(int npcSelectedByPlayerCount)
{
    vector<Vec2> npcMoveEndPositionList;
    int queueMaxRowCount = std::min(npcSelectedByPlayerCount, QUEUE_MAX_ROW_COUNT);

    auto mapSize = _tileMap->getMapSize();
    int maxMapWidthIndex = mapSize.width - 1;
    int maxMapHeightIndex = mapSize.height - 1;

    auto startSubscript = getTileSubscript();
    
    auto tileInfo = getTileInfoAt((int)startSubscript.x, (int)startSubscript.y);
    auto deltaFromCursorToTile = _tileMap->convertToNodeSpace(_cursorPoint) - tileInfo.leftTopPosition;

    startSubscript.x = std::max(startSubscript.x, 0.0f);
    startSubscript.x = std::min((int)startSubscript.x, maxMapWidthIndex);

    startSubscript.y = std::max(startSubscript.y, (float)(queueMaxRowCount / 2));
    startSubscript.y = std::min(startSubscript.y, (float)(maxMapHeightIndex - queueMaxRowCount / 2));

    int minRowIndex = startSubscript.y - queueMaxRowCount / 2;
    int maxRowIndex = startSubscript.y + queueMaxRowCount / 2;

    bool canSearchLeftArea = true;
    bool canSearchRightArea = true;

    int searchLeftAreaXSubscript = (int)startSubscript.x;
    int searchRightAreaXSubscript = (int)startSubscript.x + 1;
    while ((int)npcMoveEndPositionList.size() < npcSelectedByPlayerCount)
    {
        if (searchLeftAreaXSubscript >= 0 && canSearchLeftArea)
        {
            int count = insertNpcMoveEndPositionInto(npcMoveEndPositionList, searchLeftAreaXSubscript, minRowIndex, maxRowIndex, npcSelectedByPlayerCount, deltaFromCursorToTile);

            if (count <= 0)
            {
                canSearchLeftArea = false;
            }

            searchLeftAreaXSubscript--;
        }

        if ((int)npcMoveEndPositionList.size() >= npcSelectedByPlayerCount)
        {
            break;
        }

        if (searchRightAreaXSubscript <= maxMapWidthIndex && canSearchRightArea)
        {
            int count = insertNpcMoveEndPositionInto(npcMoveEndPositionList, searchRightAreaXSubscript, minRowIndex, maxRowIndex, npcSelectedByPlayerCount, deltaFromCursorToTile);

            if (count <= 0)
            {
                canSearchRightArea = false;
            }

            searchRightAreaXSubscript++;
        }

        if (!canSearchLeftArea && !canSearchRightArea)
        {
            break;
        }
    }

    return npcMoveEndPositionList;
}

int MapManager::insertNpcMoveEndPositionInto(vector<Vec2>& npcMoveEndPositionList, int xSubscript, int minRowIndex, int maxRowIndex, int npcSelectedByPlayerCount, Vec2 deltaFromCursorToTile)
{
    int count = 0;

    for (int ySubscript = minRowIndex; ySubscript <= maxRowIndex; ySubscript ++)
    {
        auto tileInfo = getTileInfoAt(xSubscript, ySubscript);
        if (tileInfo.gid != OBSTACLE_ID)
        {
            count++;
            npcMoveEndPositionList.push_back(tileInfo.leftTopPosition + deltaFromCursorToTile);

            if ((int)npcMoveEndPositionList.size() >= npcSelectedByPlayerCount)
            {
                break;
            }
        }
    }

    return count;
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

Size MapManager::getTileSize()
{
    return _tileMap->getTileSize();
}

Point MapManager::getTileSubscript()
{
    Point tileSubscript;

    auto mapSize = _tileMap->getMapSize();
    auto tileSize = _tileMap->getTileSize();
    auto positionInTileMap = _tileMap->convertToNodeSpace(_cursorPoint);
    tileSubscript.x = (mapSize.height - (positionInTileMap.y / tileSize.height)) + ((positionInTileMap.x / tileSize.width) - mapSize.width / 2);
    tileSubscript.y = (mapSize.height - (positionInTileMap.y / tileSize.height)) - ((positionInTileMap.x / tileSize.width) - mapSize.width / 2);

    tileSubscript.x = std::max(0.0f, tileSubscript.x);
    tileSubscript.x = std::min(mapSize.width - 1.0f, tileSubscript.x);
    tileSubscript.y = std::max(0.0f, tileSubscript.y);
    tileSubscript.y = std::min(mapSize.height - 1.0f, tileSubscript.y);

    tileSubscript.x = std::floor(tileSubscript.x);
    tileSubscript.y = std::floor(tileSubscript.y);

    return tileSubscript;
}

DebugInfo MapManager::getDebugInfo(TileMapLayerType tileMapLayerType)
{
    DebugInfo tileDebugInfo;

    auto currentTileMapLayer = _tileMap->getLayer(s_tileMapLayerTypeToString[tileMapLayerType]);
    auto tileSubscript = getTileSubscript();

    tileDebugInfo.gid = currentTileMapLayer->getTileGIDAt(tileSubscript);
    tileDebugInfo.tileMapLayerName = s_tileMapLayerTypeToString[tileMapLayerType];
    tileDebugInfo.cursorPoint = _cursorPoint;
    tileDebugInfo.tileSubscript = tileSubscript;

    return tileDebugInfo;
}

cocos2d::Vec2 MapManager::convertCursorPositionToTileMapSpace()
{
    return _tileMap->convertToNodeSpace(_cursorPoint);
}

void MapManager::addChildInGameObjectLayer(Node* gameObject, int zOrder/* = 1*/)
{
    auto gameObjectLayer = _tileMap->getLayer("gameObjectLayer");
    gameObjectLayer->addChild(gameObject, zOrder);
}

void MapManager::initTileMapInfoTable()
{
    auto mapSize = _tileMap->getMapSize();
    _tileInfoTable.resize((int)mapSize.width);

    auto tileSize = _tileMap->getTileSize();

    auto gameObjectLayer = _tileMap->getLayer(s_tileMapLayerTypeToString[TileMapLayerType::GameObjcetLayer]);

    for (int tileXSubscript = 0; tileXSubscript < (int)mapSize.width; tileXSubscript ++)
    {
        _tileInfoTable[tileXSubscript].resize((int)mapSize.height);
        for (int tileYSubscript = 0; tileYSubscript < (int)mapSize.height; tileYSubscript ++)
        {
            _tileInfoTable[tileXSubscript][tileYSubscript].gid = gameObjectLayer->getTileGIDAt(Vec2(tileXSubscript, tileYSubscript));

            Vec2 positionInTileMap;
            positionInTileMap.x = (((float)tileXSubscript - (float)tileYSubscript) / 2.0f + mapSize.width / 2.0f) * tileSize.width;
            positionInTileMap.y = (mapSize.height - ((float)tileXSubscript + (float)tileYSubscript) / 2.0f) *  tileSize.height;
            _tileInfoTable[tileXSubscript][tileYSubscript].leftTopPosition = positionInTileMap;
        }
    }
}

TileInfo MapManager::getTileInfoAt(int xSubscript, int ySubscript)
{
    return _tileInfoTable[xSubscript][ySubscript];
}
