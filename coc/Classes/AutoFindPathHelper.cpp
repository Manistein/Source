#include "Base.h"
#include "MapManager.h"
#include "AutoFindPathHelper.h"

static vector<vector<TileNode*>> g_tileNodeTable;

static TileNode* g_currentNode = nullptr;
static TileNode* g_endNode = nullptr;
static list<TileNode*> g_openList;
static list<TileNode*> g_closeList;

static int g_maxRowCount = 10;
static int g_maxColumnCount = 10;


void AutoFindPathHelper::reset()
{
    for (auto tileNodeInOpenList : g_openList)
    {
        tileNodeInOpenList->gotoEndNodeWeight = 0;
        tileNodeInOpenList->gotoStartNodeWeight = 0;
        tileNodeInOpenList->sumWeight = 0;

        tileNodeInOpenList->isVisit = false;
        tileNodeInOpenList->parent = nullptr;
    }

    for (auto tileNodeInCloseList : g_closeList)
    {
        tileNodeInCloseList->gotoEndNodeWeight = 0;
        tileNodeInCloseList->gotoStartNodeWeight = 0;
        tileNodeInCloseList->sumWeight = 0;

        tileNodeInCloseList->isVisit = false;
        tileNodeInCloseList->parent = nullptr;
    }

    g_openList.clear();
    g_closeList.clear();
}

void AutoFindPathHelper::initTileNodeTable(const vector<vector<TileNode*>>& tileNodeTable)
{
    g_tileNodeTable = tileNodeTable;
    g_maxColumnCount = (int)tileNodeTable.size();
    g_maxRowCount = (int)tileNodeTable[0].size();
}

list<TileNode*> AutoFindPathHelper::computeTileNodePathListBetween(TileNode* startNode, TileNode* endNode)
{
    list<TileNode*> pathList;

    reset();

    g_currentNode = startNode;
    g_endNode = endNode;

    startNode->isVisit = true;
    g_closeList.push_back(startNode);

    while (true)
    {
        auto nextPathNode = findNextPathNodeBeside(g_currentNode);
        if (!nextPathNode)
        {
            if (!g_openList.empty())
            {
                g_currentNode = startNode;
                continue;
            }
            else
            {
                break;
            }
        }
        
        g_openList.remove(nextPathNode);
        g_closeList.push_back(nextPathNode);
        g_currentNode = nextPathNode;

        if (nextPathNode == g_endNode)
        {
            pathList = getPathList(startNode, g_endNode);
            break;
        }
    }

    return pathList;
}

bool AutoFindPathHelper::canVisit(TileNode* node)
{
    bool result = true;

    if (node->gid == OBSTACLE_ID || 
        node->isVisit)
    {
        result = false;
    }

    return result;
}

bool isLeftSumWeightLessThanRight(const TileNode* left, const TileNode* right);
TileNode* AutoFindPathHelper::findNextPathNodeBeside(TileNode* node)
{
    TileNode* nextPathNode = nullptr;

    int minRowIndex = max(node->rowIndex - 1, 0);
    int maxRowIndex = min(node->rowIndex + 1, g_maxRowCount - 1);

    int minColumnIndex = max(node->columnIndex - 1, 0);
    int maxColumnIndex = min(node->columnIndex + 1, g_maxColumnCount - 1);

    for (int columnIndex = minColumnIndex; columnIndex <= maxColumnIndex; columnIndex++)
    {
        for (int rowIndex = minRowIndex; rowIndex <= maxRowIndex; rowIndex++)
        {
            auto prepareToVisitNode = g_tileNodeTable[columnIndex][rowIndex];
            if (canVisit(prepareToVisitNode))
            {
                prepareToVisitNode->isVisit = true;
                prepareToVisitNode->parent = node;

                if (prepareToVisitNode->rowIndex != node->rowIndex && 
                    prepareToVisitNode->columnIndex != node->columnIndex)
                {
                    prepareToVisitNode->gotoStartNodeWeight = node->gotoStartNodeWeight + MOVE_SLOP_WEIGHT;
                }
                else
                {
                    prepareToVisitNode->gotoStartNodeWeight = node->gotoStartNodeWeight + MOVE_STRAIGHT_WEIGHT;
                }

                int gotoEndNodeRowDistance = std::abs(g_endNode->rowIndex - prepareToVisitNode->rowIndex);
                int gotoEndNodeColumnDistance = std::abs(g_endNode->columnIndex - prepareToVisitNode->columnIndex);
                prepareToVisitNode->gotoEndNodeWeight = (gotoEndNodeRowDistance + gotoEndNodeColumnDistance) * MOVE_STRAIGHT_WEIGHT;

                prepareToVisitNode->sumWeight = prepareToVisitNode->gotoStartNodeWeight + prepareToVisitNode->gotoEndNodeWeight;

                g_openList.push_back(prepareToVisitNode);
            }
        }
    }

    if (!g_openList.empty())
    {
        // 按sumWeight值从小到大排序
        vector<TileNode*> alternativeNodeVector(g_openList.begin(), g_openList.end());
        std::sort(alternativeNodeVector.begin(), alternativeNodeVector.end(), isLeftSumWeightLessThanRight);
        g_openList.clear();
        g_openList.assign(alternativeNodeVector.begin(), alternativeNodeVector.end());

        for (auto alternativeNode : g_openList)
        {
            if (alternativeNode->gotoStartNodeWeight > node->gotoStartNodeWeight)
            {
                nextPathNode = alternativeNode;

                break;
            }
        }
    }

    return nextPathNode;
}

bool isLeftSumWeightLessThanRight(const TileNode* left, const TileNode* right)
{
    return left->sumWeight < right->sumWeight;
}

list<TileNode*> AutoFindPathHelper::getPathList(TileNode* startNode, TileNode* endNode)
{
    list<TileNode*> pathList;

    
    auto previousNode = endNode;
    while (previousNode)
    {
        pathList.push_back(previousNode);
        previousNode = previousNode->parent;
    }

    return pathList;
}
