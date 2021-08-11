/*
 */

#ifndef VTINPUT_H
#define VTINPUT_H

#include <Arduino.h>
#include <BasicTerm.h>
#include "VTItem.h"

class VTInput : public VTEditItem {
public:
    VTInput (int id, int row, int col, BasicTerm& term, int w, int min, int max, void (*onConfirmed)(VTEditItem*));
    VTInput (int id, int row, int col, BasicTerm& term, int w, int min, int max, int* value, void (*onConfirmed)(VTEditItem*));
    
    virtual void draw();
    virtual void apply();
    virtual bool handleKey(int key);
    virtual bool canFocus() { return true; }
    
private:
    int mWidth;
    int mMin;
    int mMax;
};

#endif
