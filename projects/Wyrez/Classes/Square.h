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
    SquareFillStateTypes m_fillState;
    SquareChargeStateTypes m_chargeState;
    
public:
    Square();
    Square(float x, float y, float width, float height);
};

#endif /* defined(__Wyrez__Square__) */
