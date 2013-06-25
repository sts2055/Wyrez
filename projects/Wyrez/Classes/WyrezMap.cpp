//
//  WyrezMap.cpp
//  Wyrez
//
//  Created by stefan on 6/24/13.
//
//

#include "WyrezMap.h"

WyrezMap::WyrezMap()
: m_squareSide(0)
, m_squaresCount_vertical(0)
, m_squaresCount_horizontal(0)
, m_contentSize(CCSizeZero)
, m_pGridOrigins_vertical(nullptr)
, m_pGridOrigins_horizontal(nullptr)
, m_pSquares(nullptr)
{
    
}

WyrezMap::~WyrezMap()
{
    CCLOG("cocos2d: deallocing WyrezMap %p", this);
    
    for (auto pPoint : *m_pGridOrigins_vertical)
    {
        delete pPoint;
    }
    delete m_pGridOrigins_vertical;
    
    for (auto pPoint : *m_pGridOrigins_horizontal)
    {
        delete pPoint;
    }
    delete m_pGridOrigins_horizontal;
    
    for (auto square : *m_pSquares)
    {
        delete square;
    }
    delete m_pSquares;
}

bool WyrezMap::init()
{
    m_squareSide = kSquareSide;
    m_squaresCount_vertical = kDefaultSquaresVertical;
    m_squaresCount_horizontal = kDefaultSquaresHorizontal;
    
    m_contentSize = CCSizeMake(m_squaresCount_vertical * kSquareSide, m_squaresCount_horizontal * kSquareSide);
    
    m_pGridOrigins_vertical = new std::vector<CCPoint*>();
    m_pGridOrigins_horizontal = new std::vector<CCPoint*>();
    m_pSquares = new std::vector<Square*>();
    
    int verticalLines = m_squaresCount_vertical;
    int horizontalLines = m_squaresCount_horizontal;
    
    for (int i = 0; i < verticalLines; i++)
    {
        m_pGridOrigins_vertical->push_back(new CCPoint(i * kSquareSide, 0));
    }
    
    for (int i = 0; i < horizontalLines; i++)
    {
        m_pGridOrigins_horizontal->push_back(new CCPoint(0, i * kSquareSide));
    }
    
    
    for (int i = 0; i < verticalLines; i++)
    {
        for (int j = 0; j < horizontalLines; j++)
        {
            m_pSquares->push_back(new Square(i * kSquareSide, j * kSquareSide, kSquareSide, kSquareSide));
        }
    }
    
    
    return true;
}