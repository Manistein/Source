#pragma once

#include "CCStdC.h"
#include <string>
#include <vector>
#include <set>
#include <list>
#include <map>
#include <functional>
using namespace std;

#include "ui/CocosGUI.h"
#include "ui/UIButton.h"
#include "cocos2d.h"

using namespace cocos2d;
using namespace ui;

#include <winuser.h>
#include <windef.h>

#include <../Libs/iconv/include/iconv.h>

const int MAX_GAME_OBJECT_COUNT = 1000;

const int FIRST_STAGE = 1;
const float MAX_VOLUME = 1.0f;

enum class DifficultyLevel
{
    Easy,
    Normal,
    Hard
};