/*
 */

#include "VTCombo.h"

VTCombo::VTCombo(int id, int row, int col, BasicTerm& term, int w, String* values, int numValues, void (*onConfirmed)(VTEditItem*))
    : VTEditItem(id, "", row, col, term, onConfirmed)
    , mWidth(w)
    , mValues(values)
    , mNumValues(numValues)
{
}

VTCombo::VTCombo(int id, int row, int col, BasicTerm& term, int w, String* values, int numValues, int* value, void (*onConfirmed)(VTEditItem*))
    : VTEditItem(id, "", row, col, term, value, onConfirmed)
    , mWidth(w)
    , mValues(values)
    , mNumValues(numValues)
{
}

void VTCombo::draw()
{
    String s = mValues[mCurrValue];
    
    mTerm.position(mRow, mCol);
    mTerm.set_attribute(BT_NORMAL);
    if (mFocused)
        mTerm.set_attribute(BT_REVERSE);

    for (int i=0; i<mWidth-s.length(); i++)
        mTerm.print(" ");

    mTerm.print(s);
}

void VTCombo::apply()
{
    *mValue = mCurrValue;
    if (mFnc)
        mFnc(this);
}

bool VTCombo::handleKey(int key)
{
    bool handled = false;
    if (!mFocused)
        return handled;
    
    if (key == 260)
    {
        if (mCurrValue > 0)
        {
            mCurrValue --;
            draw();
        }
        
        handled = true;
    }
    else if (key == 261)
    {
        if (mCurrValue < mNumValues-1)
        {
            mCurrValue ++;
            draw();
        }
        
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
