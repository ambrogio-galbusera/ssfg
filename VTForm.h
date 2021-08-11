/*
 */

#ifndef VTFORM_H
#define VTFORM_H

#include <Arduino.h>
#include <BasicTerm.h>
#include "VTLabel.h"
#include "VTOption.h"

class VTForm {
public:
    VTForm(String title, int w, int h, BasicTerm& term,
        VTItem** items, int numItems
        );
    
    void draw();
    void handleKey(int key);
    
private:
    void drawFrame();
    void drawItems();

    int firstFocusableItem();
    int nextFocusableItem(int from);
    int prevFocusableItem(int from);

    String mTitle;
    int mWidth;
    int mHeight;
    
    VTItem** mItems;
    int mNumItems;
    int mCurrItem;
    
    BasicTerm& mTerm;
};

#endif
