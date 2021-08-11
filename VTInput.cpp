/*
 */

#include "VTInput.h"

VTInput::VTInput(int id, int row, int col, BasicTerm& term, int w, int min, int max, void (*onConfirmed)(VTEditItem*))
    : VTEditItem(id, "", row, col, term, onConfirmed)
    , mWidth(w)
    , mMin(min)
    , mMax(max)
{
}

VTInput::VTInput(int id, int row, int col, BasicTerm& term, int w, int min, int max, int* value, void (*onConfirmed)(VTEditItem*))
    : VTEditItem(id, "", row, col, term, value, onConfirmed)
    , mWidth(w)
    , mMin(min)
    , mMax(max)
{
}

void VTInput::draw()
{
    String s(mCurrValue, DEC);
    
    mTerm.position(mRow, mCol);
    mTerm.set_attribute(BT_NORMAL);
    if (mFocused)
        mTerm.set_attribute(BT_REVERSE);

    for (int i=0; i<mWidth-s.length(); i++)
        mTerm.print("_");
    
    mTerm.print(s);
}

void VTInput::apply()
{
    *mValue = mCurrValue;
    if (mFnc)
        mFnc(this);
}

bool VTInput::handleKey(int key)
{
    bool handled = false;
    if (!mFocused)
        return handled;
    
    if ((key >= '0') && (key <= '9'))
    {
        mCurrValue = (mCurrValue * 10) + (key - '0');
        
        if (mCurrValue > mMax)
            mCurrValue = mMax;
        if (mCurrValue < mMin)
            mCurrValue = mMin;

        draw();
        handled = true;
    }
    else if (key == 8)
    {
        mCurrValue = mCurrValue / 10;
        draw();
        handled = true;
    }
    else if (key == 27)
    {
        mCurrValue = *mValue;
        draw();
        handled = true;
    }
    else if (key == 13)
    {
        *mValue = mCurrValue;
        if (mFnc)
            mFnc(this);
        handled = true;
    }

    return handled;
}
