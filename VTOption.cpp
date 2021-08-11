/*
 */

#include "VTOption.h"

VTOption::VTOption(int id, String text, int row, int col, BasicTerm& term, bool* value)
    : VTItem(id, text, row, col, term)
    , mValue(value)
{
}

void VTOption::draw()
{
    mTerm.position(mRow, mCol);
    mTerm.set_attribute(BT_NORMAL);
    if (mFocused)
        mTerm.set_attribute(BT_BOLD | BT_UNDERLINE);
    
    if (*mValue)
        mTerm.print("[X] ");
    else
        mTerm.print("[ ] ");
    
    mTerm.print(mText);
}

bool VTOption::handleKey(int key)
{
    bool handled = false;
    
    switch (key)
    {
        case ' ': // toggle selection
            *mValue = !(*mValue);
            draw();
            handled = true;
        break;
    }
    
    return handled;
}
