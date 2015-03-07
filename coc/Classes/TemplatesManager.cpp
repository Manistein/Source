#include "Base.h"
#include "TemplatesManager.h"
#include "TabFileReader.h"

static TemplateManager* s_templateManager = nullptr;

TemplateManager::~TemplateManager()
{
    for (auto& iter : _npcTemplatesMap)
    {
        if (iter.second)
        {
            CC_SAFE_DELETE(iter.second);
        }
    }
    _npcTemplatesMap.clear();
}

TemplateManager* TemplateManager::getInstance()
{
    if (s_templateManager == nullptr)
    {
        s_templateManager = new TemplateManager();
        s_templateManager->init();
    }

    return s_templateManager;
}

const NpcTemplate* TemplateManager::getNpcTemplateBy(const string& jobName)
{
    NpcTemplate* npcTemplate = nullptr;

    auto iter = _npcTemplatesMap.find(jobName);
    if (iter != _npcTemplatesMap.end())
    {
        npcTemplate = iter->second;
    }

    return npcTemplate;
}

bool TemplateManager::init()
{
    bool result = false;

    TabFileReader tabFileReader;
    if (tabFileReader.open("NpcTemplates.tab"))
    {
        for (int i = 0; i < tabFileReader.getRowCount(); i ++)
        {
            string jobName = tabFileReader.getString(i, "NpcJobName");

            NpcTemplate* npcTemplate = new NpcTemplate();
            npcTemplate->moveToNorthEastAnimationPList = tabFileReader.getString(i, "MoveToNorthEastAnimationPList");
            npcTemplate->moveToEastAnimationPList = tabFileReader.getString(i, "MoveToEastAnimationPList");
            npcTemplate->moveToSouthEastAnimationPList = tabFileReader.getString(i, "MoveToSouthEastAnimationPList");
            npcTemplate->moveToSouthWestAnimationPList = tabFileReader.getString(i, "MoveToSouthWestAnimationPList");
            npcTemplate->moveToWestAnimationPList = tabFileReader.getString(i, "MoveToWestAnimationPList");
            npcTemplate->moveToNorthWestAnimationPList = tabFileReader.getString(i, "MoveToNorthWestAnimationPList");

            npcTemplate->dieAnimationPList = tabFileReader.getString(i, "DieAnimationPList");

            npcTemplate->standAndFaceToEastAnimationPList = tabFileReader.getString(i, "StandAndFaceToEastPList");
            npcTemplate->standAndFaceToNorthEastAnimationPList = tabFileReader.getString(i, "StandAndFaceToNorthEastPList");
            npcTemplate->standAndFaceToNorthWestAnimationPList = tabFileReader.getString(i, "StandAndFaceToNorthWestPList");
            npcTemplate->standAndFaceToSouthEastAnimationPList = tabFileReader.getString(i, "StandAndFaceToSouthEastPList");
            npcTemplate->standAndFaceToSouthWestAnimationPList = tabFileReader.getString(i, "StandAndFaceToSouthWestPList");
            npcTemplate->standAndFaceToWestAnimationPList = tabFileReader.getString(i, "StandAndFaceToWestPList");

            _npcTemplatesMap[jobName] = npcTemplate;
        }

        result = true;
    }

    return result;
}
