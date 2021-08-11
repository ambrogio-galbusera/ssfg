/*
 */

#ifndef VTMENUITEM_H
#define VTMENUITEM_H

#include <Arduino.h>
#include <BasicTerm.h>
#include "VTItem.h"

class VTMenuItem : public VTItem {
public:
    VTMenuItem (int id, String text, int row, int col, BasicTerm& term, void (*onSelected)(VTItem*));
    
    virtual void draw();
    virtual bool handleKey(int key);
    virtual bool canFocus() { return true; }
    
private:
    void (*mFnc)(VTItem*);
    int mShortcut;
    int mShortcutIndex;
};

#endif
