//
//  Square.h
//  Wyrez
//
//  Created by stefan on 6/21/13.
//
//

#ifndef __Wyrez__Square__
#define __Wyrez__Square__

#include "cocos2d.h"

USING_NS_CC;

typedef enum {
    kSquareFillStateEmpty = 0,
    kSquareFillStateFilled = 1,
} SquareFillStateTypes;

typedef enum {
    kSquareChargeStateNoCharge = 0,
    kSquareChargeStateCharged = 1,
    kSquareChargeStateDischarging = 2,
} SquareChargeStateTypes;

class Square : public CCRect
{
public:
    int m_index;
    SquareFillStateTypes m_fillState;
    SquareChargeStateTypes m_chargeState;
    std::vector<Square*> * m_surroundingSquares;
    
public:
    Square();
    Square(int index, float x, float y, float width, float height);
    virtual ~Square();
    
    // Overloaded < for comparisons based on the square's index (e.g. used when sorting).
    // The index corresponds to its location on the map since when generating the map grid,
    // squares are positioned and push_back'ed consecutively starting from 0,0 (bottom left)
    // whilst first filling the column (going up) before moving on to the next column to the right.
    bool operator<( const Square& val ) const { return m_index < val.m_index; }
        
    bool operator!=( const Square& val ) const { return m_index != val.m_index; }
};

#endif /* defined(__Wyrez__Square__) */
