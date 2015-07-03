#pragma once

class WindowsHelper
{
public:
    ~WindowsHelper();
    static WindowsHelper* getInstance();

    void switchToAttackCursor();
    void switchToNormalCursor();
    void uninitInstane();
private:
    bool init();
    
    HCURSOR _attackCursorHandle;
    HCURSOR _normalCursorHandle;
    HCURSOR _systemDefaultCursor;

    WindowsHelper(){};
    WindowsHelper(const WindowsHelper&);
    WindowsHelper& operator = (const WindowsHelper&);
};