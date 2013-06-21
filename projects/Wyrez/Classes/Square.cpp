//
//  Square.cpp
//  Wyrez
//
//  Created by stefan on 6/21/13.
//
//

#include "Square.h"

Square::Square()
: m_fillState(kSquareFillStateEmpty)
, m_chargeState(kSquareChargeStateNoCharge)
{
    setRect(0.0f, 0.0f, 0.0f, 0.0f);
}

Square::Square(float x, float y, float width, float height)
: m_fillState(kSquareFillStateEmpty)
, m_chargeState(kSquareChargeStateNoCharge)
{
    setRect(x, y, width, height);
}