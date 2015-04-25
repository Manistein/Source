#pragma once

const int MOVE_SLOP_WEIGHT = 14;
const int MOVE_STRAIGHT_WEIGHT = 10;

class AutoFindPathHelper
{
public:
    static void initTileNodeTable(const vector<vector<TileNode*>>& tileNodeTable);
    static list<TileNode*> computeTileNodePathListBetween(TileNode* startNode, TileNode* endNode);

private:
    static void reset();

    static bool canVisit(TileNode* node);
    static TileNode* findNextPathNodeBeside(TileNode* node);
    static list<TileNode*> getPathList(TileNode* startNode, TileNode* endNode);

    AutoFindPathHelper();
    ~AutoFindPathHelper();
};
