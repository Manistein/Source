#include "Base.h"
#include "MapManager.h"

const float MAP_MOVE_SPEED = 10.0f;
const float MAP_BORDER_MARGIN = 50.0f;
const float MAP_MIN_SCALE = 0.5f;
const float MAP_MAX_SCALE = 1.0f;

bool MapManager::init(Node* parentNode, const std::string& titleMapFileName)
{
    auto glView = Director::getInstance()->getOpenGLView();
    auto windowHandle = glView->getWin32Window();
    GetWindowRect(windowHandle, &_clientRect);

    _titleMap = TMXTiledMap::create(titleMapFileName);
    _titleMap->setScale(_mapScale);
    parentNode->addChild(_titleMap);

    return true;
}

void MapManager::updateMapPosition()
{
    auto titleMapPosition = _titleMap->getPosition();
    auto titleMapSize = _titleMap->getContentSize() * _titleMap->getScale();

    if (_cursorPosition.x <= _clientRect.left + MAP_BORDER_MARGIN)
    {
        titleMapPosition.x += MAP_MOVE_SPEED;
    }

    if (_cursorPosition.y <= _clientRect.top + MAP_BORDER_MARGIN)
    {
        titleMapPosition.y += MAP_MOVE_SPEED;
    }

    if (_cursorPosition.x >= _clientRect.right - MAP_BORDER_MARGIN)
    {
        titleMapPosition.x -= MAP_MOVE_SPEED;
    }

    if (_cursorPosition.y >= _clientRect.bottom - MAP_BORDER_MARGIN)
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

    _titleMap->setPosition(titleMapPosition);
}

void MapManager::updateMapScale(Event* event)
{
    auto mouseEvent = static_cast<EventMouse*>(event);
    if (mouseEvent->getScrollY() == 1)
    {
        _mapScale = std::min(_mapScale + 0.1f, MAP_MAX_SCALE);
    }
    else
    {
        _mapScale = std::max(_mapScale - 0.1f, MAP_MIN_SCALE);
    }

    _titleMap->setScale(_mapScale);
}

void MapManager::updateCursorPosition(Event* event)
{
    auto mouseEvent = static_cast<EventMouse*>(event);
    auto clientHeight = _clientRect.bottom - _clientRect.top;

    _cursorPosition.x = mouseEvent->getCursorX();
    _cursorPosition.y = clientHeight + mouseEvent->getCursorY();
}

cocos2d::Vec2 MapManager::getCursorPosition()
{
    return _cursorPosition;
}
