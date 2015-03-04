#include "Base.h"
#include "base\CCData.h"
#include "platform\CCFileUtils.h"
#include "TabFileReader.h"

#define TAB_FILE_SEPERATE_TOKEN '\t'
#define COLUMN_NOT_FOUND -1

TabFileReader::TabFileReader()
{
}

TabFileReader::~TabFileReader()
{
    clear();
}


bool TabFileReader::open(const char* fileName)
{
    bool result = false;
    std::string filePath = FileUtils::getInstance()->fullPathForFilename(fileName);

    if (FileUtils::getInstance()->isFileExist(filePath))
    {
        Data data = FileUtils::getInstance()->getDataFromFile(filePath);
        result = parse(data);
    }
         
    return result;
}

std::string TabFileReader::getString(int rowIndex, int columnIndex)
{
    std::string cellString;

    if (rowIndex < rowCount && columnIndex < columnCount)
    {
        cellString = _data[rowIndex][columnIndex];
    }
    return cellString;
}

int TabFileReader::getInteger(int rowIndex, int columnIndex)
{
    int cellInteger = 0;

    if (rowIndex < rowCount && columnIndex < columnCount)
    {
        cellInteger = atoi(_data[rowIndex][columnIndex].c_str());
    }

    return cellInteger;
}

float TabFileReader::getFloat(int rowIndex, int columnIndex)
{
    float result = 0.0f;

    if (rowIndex < rowCount && columnIndex < columnCount)
    {
        result = atof(_data[rowIndex][columnIndex].c_str());
    }

    return result;
}

std::string TabFileReader::getString(int rowIndex, const char* columnName)
{
    std::string result;

    int columnIndex = getColumnIndex(columnName);    
    CCASSERT(columnIndex != COLUMN_NOT_FOUND, "ColumnName not found");

    if (columnIndex != COLUMN_NOT_FOUND)
    {
        result = getString(rowIndex, columnIndex);
    }

    return result;
}

int TabFileReader::getInteger(int rowIndex, const char* columnName)
{
    int result;

    int columnIndex = getColumnIndex(columnName);
    CCASSERT(columnIndex != COLUMN_NOT_FOUND, "ColumnName not found");

    if (columnIndex != COLUMN_NOT_FOUND)
    {
        result = getInteger(rowIndex, columnIndex);
    }

    return result;
}

float TabFileReader::getFloat(int rowIndex, const char* columnName)
{
    float result;

    int columnIndex = getColumnIndex(columnName);
    CCASSERT(columnIndex != COLUMN_NOT_FOUND, "ColumnName not found");

    if (columnIndex != COLUMN_NOT_FOUND)
    {
        result = getFloat(rowIndex, columnIndex);
    }

    return result;
}

int TabFileReader::getRowCount()
{
    return rowCount;
}

int TabFileReader::getColumnCount()
{
    return columnCount;
}

bool TabFileReader::parse(const Data& data)
{
    bool result = false;
    int dataSize = data.getSize();
    if (dataSize == 0)
    {
        return result;
    }

    char* fileData = (char*)data.getBytes();
  
    int beginPosition = 0;
    int endPosition = 0;
    int rowIndex = -1;
    
    while (endPosition < dataSize && beginPosition < dataSize)
    {
        if ((fileData[endPosition + 1] == '\r' && fileData[endPosition + 2] == '\n') ||
            endPosition + 1 == dataSize )
        {
            if (rowIndex == -1)
            {
                parseTitleLine(fileData, beginPosition, endPosition);
                rowIndex++;
            }
            else
            {
                parseDataLine(fileData, beginPosition, endPosition);
            }

            if (endPosition < dataSize && fileData[endPosition + 1] == '\r' && fileData[endPosition + 2] == '\n')
            {
                beginPosition = endPosition + strlen("\r\n") + 1 ;
            }            
        }
        
        endPosition++;                
    }    

    rowCount = _data.size();

    result = true;

    return result;
}

void TabFileReader::parseTitleLine(char* fileData, int beginPosition, int endPosition)
{
    return parseLine(fileData, beginPosition, endPosition, true);
}

void TabFileReader::parseDataLine(char* fileData, int beginPosition, int endPosition)
{
    return parseLine(fileData, beginPosition, endPosition, false);
}

void TabFileReader::parseLine(char* fileData, int beginPosition, int endPosition, bool isTitleLine)
{
    std::vector<std::string>  columns;
    int cellStringBeginPosition = beginPosition;
    for (int currentPosition = cellStringBeginPosition; currentPosition <= endPosition; currentPosition++)
    {
        std::string cellString;

        if (fileData[currentPosition] == TAB_FILE_SEPERATE_TOKEN)
        {
            cellString = std::string(fileData + cellStringBeginPosition, currentPosition - cellStringBeginPosition);            
            
            cellStringBeginPosition = currentPosition + 1;
        }
        else if (currentPosition == endPosition)
        {
            cellString = std::string(fileData + cellStringBeginPosition, currentPosition - cellStringBeginPosition + 1);            
        }

        if (cellString.size() > 0)
        {
            columns.push_back(cellString);

            if (isTitleLine)
            {
                _columnName2IndexMap[cellString] = columns.size() - 1;
            }
        }        
    }

    if (isTitleLine)
    {
        columnCount = columns.size();
    }
    else
    {
        _data.push_back(columns);
    }    
}

void TabFileReader::clear()
{
}

int TabFileReader::getColumnIndex(const char* columnName)
{
    int result = COLUMN_NOT_FOUND;
    
    auto findResult = _columnName2IndexMap.find(std::string(columnName));
    if ( findResult != _columnName2IndexMap.end())
    {
        result = findResult->second;
    }

    return result;
}