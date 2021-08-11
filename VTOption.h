/*
 */

#ifndef VTOPTION_H
#define VTOPTION_H

#include <Arduino.h>
#include <BasicTerm.h>
#include "VTItem.h"

class VTOption : public VTItem {
public:
    VTOption (int id, String text, int row, int col, BasicTerm& term, bool* value);
    
    virtual void draw();
    virtual bool handleKey(int key);
    virtual bool canFocus() { return true; }
    
private:
    bool* mValue;
};

#endif
