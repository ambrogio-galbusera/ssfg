/*
 */

#ifndef VTCOMBO_H
#define VTCOMBO_H

#include <Arduino.h>
#include <BasicTerm.h>
#include "VTItem.h"

class VTCombo : public VTEditItem {
public:
    VTCombo (int id, int row, int col, BasicTerm& term, int w, String* values, int numValues, void (*onConfirmed)(VTEditItem*));
    VTCombo (int id, int row, int col, BasicTerm& term, int w, String* values, int numValues, int* value, void (*onConfirmed)(VTEditItem*));
    
    virtual void draw();
    virtual void apply();
    virtual bool handleKey(int key);
    virtual bool canFocus() { return true; }
    
private:
    int mWidth;
    String* mValues;
    int mNumValues;
};

#endif
