#pragma once

struct GameSetting
{
    bool allowDebugDraw = false;
    string fontName = "arial.ttf";
};

extern GameSetting g_setting;