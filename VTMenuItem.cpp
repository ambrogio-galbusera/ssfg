/*
 */

#include "VTMenuItem.h"

VTMenuItem::VTMenuItem(int id, String text, int row, int col, BasicTerm& term, void (*onSelected)(VTItem*))
    : VTItem(id, text, row, col, term)
    , mFnc(onSelected)
    , mShortcut(-1)
{
    mShortcutIndex = mText.indexOf('&');
    if (mShortcutIndex != -1)
    {
        mShortcut = mText[mShortcutIndex+1];
        mText.remove(mShortcutIndex, 1);
    }
}

void VTMenuItem::draw()
{
    mTerm.position(mRow, mCol);
    mTerm.set_attribute(BT_NORMAL);
    if (mFocused)
        mTerm.set_attribute(BT_BOLD | BT_UNDERLINE);
    
    mTerm.print(mText);

    if (mShortcutIndex != -1)
    {
        mTerm.position(mRow, mCol+mShortcutIndex);
        mTerm.set_attribute(BT_REVERSE);
        mTerm.print(mText.substring(mShortcutIndex, mShortcutIndex+1));
        mTerm.set_attribute(BT_NORMAL);
    }
}

bool VTMenuItem::handleKey(int key)
{
    bool handled = false;
    
    if ( ((mShortcut != -1) && (key == mShortcut)) || (mFocused && (key == 13)) )
    {
        if (mFnc != NULL)
        {
            mFnc(this);
            handled = true;
        }
    }
    
    return handled;
}
