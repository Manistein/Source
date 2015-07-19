#include "Base.h"
#include "Utils.h"
#include "base/ccMacros.h"

static map<string, Animation*> s_animationMap;

float GameUtils::computeRotatedDegree(const Vec2& beginPosition, const Vec2& endPosition)
{
    float rotation = 0.0f;

    auto moveVector = endPosition - beginPosition;
    if (moveVector.x > -FLT_EPSILON && moveVector.x < FLT_EPSILON)
    {
        if (moveVector.y >= 0.0f)
        {
            rotation = 0.0f;
        }
        else
        {
            rotation = 180.0f;
        }

        return rotation;
    }
    else if (moveVector.y > -FLT_EPSILON && moveVector.y < FLT_EPSILON)
    {
        if (moveVector.x >= 0.0f)
        {
            rotation = 90.0f;
        }
        else
        {
            rotation = 270.0f;
        }

        return rotation;
    }

    float tanTheta = moveVector.x / moveVector.y;
    if (tanTheta > 0.0f)
    {
        if (moveVector.y > 0.0f)
        {
            rotation = atan(tanTheta) * 180.0f / M_PI;
        }
        else
        {
            rotation = atan(tanTheta) * 180.0f / M_PI + 180.0f;
        }
    }
    else
    {
        if (moveVector.x > 0.0f)
        {
            rotation = atan(tanTheta) * 180.0f / M_PI + 180.0f;
        }
        else
        {
            rotation = atan(tanTheta) * 180.0f / M_PI + 360.0f;
        }
    }

    return rotation;
}


float GameUtils::computeDistanceBetween(const Vec2& beginPosition, const Vec2& endPosition)
{
    float distance = sqrt((endPosition.x - beginPosition.x) * (endPosition.x - beginPosition.x) +
        (endPosition.y - beginPosition.y) * (endPosition.y - beginPosition.y));

    return distance;
}

bool GameUtils::isFloatEqual(float left, float right)
{
    bool result = false;

    float difference = left - right;
    if (difference > -FLT_EPSILON && difference < FLT_EPSILON)
    {
        result = true;
    }

    return result;
}

bool GameUtils::isVec2Equal(const Vec2& left, const Vec2& right)
{
    bool result = false;

    if (GameUtils::isFloatEqual(left.x, right.x) && GameUtils::isFloatEqual(left.y, right.y))
    {
        result = true;
        return result;
    }

    return result;
}

Animation* parseAnimationValueMap(ValueMap& plistDataMap)
{
    auto spriteFrameCache = SpriteFrameCache::getInstance();

    auto animation = Animation::create();

    auto framesIter = plistDataMap.find("frames");
    CCASSERT(framesIter != plistDataMap.end(), "plist file has not frames key");

    auto& framesMap = plistDataMap["frames"].asValueMap();
    vector<string> frameKeysList;
    for (auto& iter : framesMap)
    {
        frameKeysList.push_back(iter.first);
    }

    std::sort(frameKeysList.begin(), frameKeysList.end());

    for (auto& key : frameKeysList)
    {
        auto spriteFrame = spriteFrameCache->getSpriteFrameByName(key);
        if (spriteFrame)
        {
            animation->addSpriteFrame(spriteFrame);
        }
    }

    return animation;
}

Animation* GameUtils::createAnimationWithPList(const string& plistFileName)
{
    Animation* animation = nullptr;

    auto fileUtils = FileUtils::getInstance();
    CCASSERT(fileUtils->isFileExist(plistFileName), StringUtils::format("%s is invalid", plistFileName.c_str()).c_str());

    auto animationIter = s_animationMap.find(plistFileName);
    if (animationIter == s_animationMap.end())
    {
        auto spriteFrameCache = SpriteFrameCache::getInstance();
        spriteFrameCache->addSpriteFramesWithFile(plistFileName);

        ValueMap plistDataMap = fileUtils->getValueMapFromFile(plistFileName);

        animation = parseAnimationValueMap(plistDataMap);
        s_animationMap[plistFileName] = animation;
        animation->retain();
    }
    else
    {
        animation = animationIter->second->clone();
    }
    
    return animation;
}

void GameUtils::pauseTargetAndChildren(Node* target)
{
    target->pause();

    auto& children = target->getChildren();
    for (auto child : children)
    {
        pauseTargetAndChildren(child);
    }
}

bool GameUtils::convertEncoding(const string& toCode, const string& fromCode, const string& fromCodeText, string& toCodeText)
{
    bool result = true;

    iconv_t openCode = iconv_open(toCode.c_str(), fromCode.c_str());

    char toText[4096] = { 0 };
    char* toTextPointer = &toText[0];
    size_t toTextMaxSize = 4096;

    const char* constFromCodeText= fromCodeText.c_str();
    size_t fromCodeTextSize = fromCodeText.length();
    size_t convertResult = iconv(openCode, &constFromCodeText, &fromCodeTextSize, &toTextPointer, &toTextMaxSize);

    if (convertResult == -1)
    {
        auto error = *_errno();
        switch (error)
        {
        case EILSEQ:
            log("Input conversion stopped due to an input byte that does not belong to the input codeset");
            break;
        case E2BIG:
            log("Input conversion stopped due to lack of space in the output buffer.");
            break;
        case EINVAL:
            log("Input conversion stopped due to an incomplete character or shift sequence at the end of the input buffer.");
            break;
        case EBADF:
            log("The cd argument is not a valid open conversion descriptor.");
            break;
        default:    break;
        }

        result = false;
    }
    else
    {
        toCodeText.assign(toText);
    }

    int closeCode = iconv_close(openCode);
    if (closeCode == -1)
    {
        result = false;
    }

    return result;
}
