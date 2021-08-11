/*
 */

#include "VTForm.h"

VTForm::VTForm(String title, int w, int h, 
    BasicTerm& term,
    VTItem** items,
    int numItems)
    : mTitle(title)
    , mWidth(w)
    , mHeight(h)
    , mItems(items)
    , mNumItems(numItems)
    , mTerm(term)
{
    mCurrItem = firstFocusableItem();
    if (mCurrItem != -1)
        mItems[mCurrItem]->setFocus(true);
}

void VTForm::draw()
{
    mTerm.cls();
    drawFrame();
    drawItems();
}

void VTForm::drawFrame()
{
    int numLDashes = (mWidth - mTitle.length() - 2) / 2;
    
    /*
    mTerm.position(0, 0);
    mTerm.set_attribute(BT_NORMAL);
    mTerm.print("+");
    for (int i=1; i<numLDashes; i++)
      mTerm.print("-");
    
    mTerm.print(" ");
    mTerm.print(mTitle);
    mTerm.print(" ");

    int numRDashes = numLDashes + (mTitle.length() % 2);
    for (int i=1; i<numRDashes; i++)
      mTerm.print("-");
    mTerm.print("+");
    
    for (int row=1; row<mHeight-1; row++)
    {
        mTerm.position(row, 0);
        mTerm.print("|");
        mTerm.position(row, mWidth-1);
        mTerm.print("|");
    }
    
    mTerm.position(mHeight-1, 0);
    mTerm.print("+");
    for (int i=1; i<mWidth-1; i++)
      mTerm.print("-");
    mTerm.print("+");
    */

    mTerm.cls();
    mTerm.position(0, 0);
    mTerm.set_attribute(BT_NORMAL);
    mTerm.print("+");
    for (int i=1; i<mWidth-1; i++)
      mTerm.print("-");
    mTerm.print("+");

    mTerm.position(1, 0);
    mTerm.print("|");
    mTerm.position(1, numLDashes);
    mTerm.print(mTitle);
    mTerm.position(1, mWidth-1);
    mTerm.print("|");

    mTerm.position(2, 0);
    mTerm.set_attribute(BT_NORMAL);
    mTerm.print("+");
    for (int i=1; i<mWidth-1; i++)
      mTerm.print("-");
    mTerm.print("+");

    for (int row=3; row<mHeight-1; row++)
    {
        mTerm.position(row, 0);
        mTerm.print("|");
        mTerm.position(row, mWidth-1);
        mTerm.print("|");
    }
    
    mTerm.position(mHeight-1, 0);
    mTerm.print("+");
    for (int i=1; i<mWidth-1; i++)
      mTerm.print("-");
    mTerm.print("+");
}

void VTForm::drawItems()
{
    if ((mItems == NULL) || (mNumItems == 0))
        return;
    
    for (int i=0; i<mNumItems; i++)
        mItems[i]->draw();
}

int VTForm::firstFocusableItem()
{
    if ((mItems == NULL) || (mNumItems == 0))
        return -1;
    
    for (int i=0; i<mNumItems; i++)
    {
        if (mItems[i]->canFocus())
            return i;
    }
    
    return -1;
}

int VTForm::nextFocusableItem(int from)
{
    if ((mItems == NULL) || (mNumItems == 0))
        return -1;
    
    int cntr = 0;
    int idx = (from + 1) % mNumItems;
    while (cntr < mNumItems)
    {
        if (mItems[idx]->canFocus())
            return idx;
        
        idx = (idx + 1) % mNumItems;
        cntr ++;
    }
    
    return -1;
}

int VTForm::prevFocusableItem(int from)
{
    if ((mItems == NULL) || (mNumItems == 0))
        return -1;
    
    int cntr = 0;
    int idx = from-1;
    if (idx < 0) idx = mNumItems-1;
    
    while (cntr < mNumItems)
    {
        if (mItems[idx]->canFocus())
            return idx;
        
        idx --;
        if (idx < 0) idx = mNumItems-1;
        
        cntr ++;
    }
    
    return -1;
}

void VTForm::handleKey(int key)
{
    /*
    mTerm.position(mHeight,0);
    mTerm.print(key);
    */
    
    bool handled;
    
    switch (key)
    {
        case 259: // up
            if (mCurrItem != -1)
            {
                mItems[mCurrItem]->setFocus(false);
                mItems[mCurrItem]->draw();
                mCurrItem = prevFocusableItem(mCurrItem);
                mItems[mCurrItem]->setFocus(true);
                mItems[mCurrItem]->draw();
            }
            break;
        case 258: // down
            if (mCurrItem != -1)
            {
                mItems[mCurrItem]->setFocus(false);
                mItems[mCurrItem]->draw();
                mCurrItem = nextFocusableItem(mCurrItem);
                mItems[mCurrItem]->setFocus(true);
                mItems[mCurrItem]->draw();
            }
            break;
        case 'B': // enter
            break;
        default:
            if (mCurrItem != -1)
            {
                handled = mItems[mCurrItem]->handleKey(key);
                if (!handled)
                {
                    for (int i=0; i<mNumItems; i++)
                    {
                        if (mItems[i]->handleKey(key))
                        {
                            mItems[mCurrItem]->setFocus(false);
                            mItems[mCurrItem]->draw();
                            mCurrItem = i;
                            mItems[mCurrItem]->setFocus(true);
                            mItems[mCurrItem]->draw();
                            
                            break;
                        }
                    }
                }
            }
            break;
       
    }
}

