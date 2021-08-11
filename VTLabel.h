/*
 */

#ifndef VTLABEL_H
#define VTLABEL_H

#include <Arduino.h>
#include <BasicTerm.h>
#include "VTItem.h"

class VTLabel : public VTItem {
public:
    VTLabel(String text, int row, int col, BasicTerm& term);
    
    virtual void draw();
    virtual bool handleKey(int key) { return false; }
    
private:
};

#endif
