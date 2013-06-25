//
//  Square.cpp
//  Wyrez
//
//  Created by stefan on 6/21/13.
//
//

#include "Square.h"

Square::Square()
: m_index(0)
, m_surroundingSquares(new std::vector<Square*>)
, m_fillState(kSquareFillStateEmpty)
, m_chargeState(kSquareChargeStateNoCharge)
{
    setRect(0.0f, 0.0f, 0.0f, 0.0f);
}

Square::Square(int index, float x, float y, float width, float height)
: m_index(index)
, m_surroundingSquares(new std::vector<Square*>)
, m_fillState(kSquareFillStateEmpty)
, m_chargeState(kSquareChargeStateNoCharge)
{
    setRect(x, y, width, height);
}

Square::~Square()
{
    delete m_surroundingSquares;
}