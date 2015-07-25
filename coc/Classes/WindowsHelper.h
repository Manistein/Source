#pragma once

enum class CursorType
{
    Invalid,
    Normal,
    Attack,
};

class WindowsHelper
{
public:
    ~WindowsHelper();
    static WindowsHelper* getInstance();

    void switchToAttackCursor();
    void switchToNormalCursor();
    void uninitInstane();

    CursorType getCursorType();
private:
    bool init();
    
    HCURSOR _attackCursorHandle;
    HCURSOR _normalCursorHandle;
    HCURSOR _systemDefaultCursor;

    CursorType _cursorType = CursorType::Invalid;

    WindowsHelper(){};
    WindowsHelper(const WindowsHelper&);
    WindowsHelper& operator = (const WindowsHelper&);
};