/*
 */

#include "VTLabel.h"

VTLabel::VTLabel(String text, int row, int col, BasicTerm& term)
    : VTItem(-1, text, row, col, term)
{
}

void VTLabel::draw()
{
    mTerm.position(mRow, mCol);
    mTerm.set_attribute(BT_NORMAL);
    mTerm.print(mText);
}
