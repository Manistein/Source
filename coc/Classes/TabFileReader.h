#pragma once

/*
YCTabFile

FirstLine is Title, which will omit.

rowIndex start from 0, columnIndex from 0

Title: Index\tName  \tID
Row0:  1    \tTest  \t1

RowCount = 1
ColumnCount = 3
*/

class TabFileReader
{
public:
    TabFileReader();
    ~TabFileReader();
    
    bool open(const char* fileName);       

    std::string getString(int rowIndex, const char* columnName);
    int getInteger(int rowIndex, const char* columnName);
    float getFloat(int rowIndex, const char* columnName);

    int getRowCount();
    int getColumnCount();

private:

    std::string getString(int rowIndex, int columnIndex);
    int getInteger(int rowIndex, int columnIndex);
    float getFloat(int rowIndex, int columnIndex);

    bool parse(const cocos2d::Data& data);
    void parseTitleLine(char* fileData, int startPosition, int endPosition);
    void parseDataLine(char* fileData, int startPosition, int endPosition);
    void parseLine(char* fileData, int startPosition, int endPosition, bool isTitleLine);
    void clear();

    int getColumnIndex(const char* columnName);

    std::vector<std::vector<std::string>> _data;
    int rowCount = 0;
    int columnCount = 0;

    std::map<std::string, int> _columnName2IndexMap;
};

