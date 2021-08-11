/*
 */

#ifndef VTITEM_H
#define VTITEM_H

#include <Arduino.h>
#include <BasicTerm.h>

class VTItem {
public:
    VTItem(int id, String text, int row, int col, BasicTerm& term)
        : mId(id)
        , mText(text)
        , mRow(row)
        , mCol(col)
        , mTerm(term)
        , mFocused(false)
    {}

    int id() { return mId; }
    
    virtual void draw() {};
    virtual void apply() {};
    virtual bool handleKey(int key) { return false; }
    virtual bool canFocus() { return false; }
    virtual void setFocus(bool focus) { mFocused = focus; if (!focus) apply(); }
    
protected:
    int mId;
    String mText;
    int mRow;
    int mCol;
    bool mFocused;
    BasicTerm& mTerm;
};

class VTEditItem : public VTItem {
public:
    VTEditItem(int id, String text, int row, int col, BasicTerm& term, void (*onConfirmed)(VTEditItem*))
        : VTItem(id, text, row, col, term)
        , mFnc(onConfirmed)
    {}

    VTEditItem(int id, String text, int row, int col, BasicTerm& term, int* value, void (*onConfirmed)(VTEditItem*))
        : VTItem(id, text, row, col, term)
        , mFnc(onConfirmed)
    {
        setValuePtr(value);
    }

    virtual void setValuePtr(int* value) {
        mValue = value;
        mCurrValue = *mValue;
    }

protected:
    int mCurrValue;
    int* mValue;
    void (*mFnc)(VTEditItem*);
};


#endif
