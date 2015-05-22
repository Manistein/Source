#pragma once

class SpecialEffectManager
{
public:
    static SpecialEffectManager* getInstance();
    Sprite* createSpecialEffect(const string& templateName, const Vec2& inMapPosition, bool isRepeat);
private:
    bool init();
    void onRemoveSpecialEffect(Node* specialEffect);

    SpecialEffectManager(){};
    SpecialEffectManager(const SpecialEffectManager&);
    SpecialEffectManager& operator = (const SpecialEffectManager&);
};