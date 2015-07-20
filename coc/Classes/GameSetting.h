#pragma once

struct GameSetting
{
    bool allowDebugDraw = false;
    string fontName = "arial.ttf";
    Color4F playerForceColor = Color4F(248.0f / 255.0f, 200.0f / 255.0f, 40.0f / 255.0f, 1.0f);
    Color4F aiForceColor = Color4F(164.0f / 255.0f, 72.0f / 255.0f, 192.0f / 255.0f, 1.0f);
    int maxStage = FIRST_STAGE;
    DifficultyLevel difficultyLevel = DifficultyLevel::Normal;
};

extern GameSetting g_setting;