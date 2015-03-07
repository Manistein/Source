#pragma once

struct NpcTemplate 
{
    string moveToNorthEastAnimationPList;
    string moveToEastAnimationPList;
    string moveToSouthEastAnimationPList;
    string moveToSouthWestAnimationPList;
    string moveToWestAnimationPList;
    string moveToNorthWestAnimationPList;

    string dieAnimationPList;

    string standAndFaceToNorthEastAnimationPList;
    string standAndFaceToEastAnimationPList;
    string standAndFaceToSouthEastAnimationPList;
    string standAndFaceToSouthWestAnimationPList;
    string standAndFaceToWestAnimationPList;
    string standAndFaceToNorthWestAnimationPList;
};

class TemplateManager
{
public:
    ~TemplateManager();

    static TemplateManager* getInstance();
    const NpcTemplate* getNpcTemplateBy(const string& jobName);
private:
    bool init();

    map<string, NpcTemplate*> _npcTemplatesMap;

    TemplateManager(){}
    TemplateManager(const TemplateManager&);
    TemplateManager& operator = (const TemplateManager&);
};