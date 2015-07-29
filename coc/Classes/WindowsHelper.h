#pragma once

enum class CursorType
{
    Invalid,
    Default,
    Normal,
    Attack,
};

class WindowsHelper
{
public:
    ~WindowsHelper();
    static WindowsHelper* getInstance();

    void switchToDefaultCursor();
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