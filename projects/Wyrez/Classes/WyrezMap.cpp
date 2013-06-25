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
, m_squaresCount_total(0)
, m_contentSize(CCSizeZero)
, m_pGridOrigins_vertical(nullptr)
, m_pGridOrigins_horizontal(nullptr)
, m_pSquares_all(nullptr)
, m_pSquares_filled(nullptr)
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
    
    for (auto square : *m_pSquares_all)
    {
        delete square;
    }
    delete m_pSquares_all;
    delete m_pSquares_filled;
}

bool WyrezMap::init()
{
    m_squareSide = kSquareSide;
    m_squaresCount_vertical = kDefaultSquaresVertical;
    m_squaresCount_horizontal = kDefaultSquaresHorizontal;
    m_squaresCount_total = m_squaresCount_vertical * m_squaresCount_horizontal;
    
    m_contentSize = CCSizeMake(m_squaresCount_vertical * kSquareSide, m_squaresCount_horizontal * kSquareSide);
    
    m_pGridOrigins_vertical = new std::vector<CCPoint*>();
    m_pGridOrigins_horizontal = new std::vector<CCPoint*>();
    m_pSquares_all = new std::vector<Square*>();
    m_pSquares_filled = new std::map<int, Square*>();
    
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
    
    // This makes the axis for vertical lines (x) the first axis when determining the index of a square inside the array
    // e.g. assuming the map is 100x100 squares (0-based), a x-value of 3 means that the square is within an index range of 300 - 400
    int index = 0;
    for (int i = 0; i < verticalLines; i++)
    {
        for (int j = 0; j < horizontalLines; j++)
        {
            m_pSquares_all->push_back(new Square(index, i * kSquareSide, j * kSquareSide, kSquareSide, kSquareSide));
            index++;
        }
    }
    this->setupSurroundingSquares();
    
    // Setup the first squares TEMP
    Square* square0 = m_pSquares_all->at(1);
    square0->m_fillState = kSquareFillStateFilled;
    square0->m_chargeState = kSquareChargeStateDischarging;
    m_pSquares_filled->insert(std::make_pair(square0->m_index, square0));
    
    Square* square1 = m_pSquares_all->at(2);
    square1->m_fillState = kSquareFillStateFilled;
    square1->m_chargeState = kSquareChargeStateCharged;
    m_pSquares_filled->insert(std::make_pair(square1->m_index, square1));
    
    Square* square2 = m_pSquares_all->at(3 + m_squaresCount_horizontal);
    square2->m_fillState = kSquareFillStateFilled;
    m_pSquares_filled->insert(std::make_pair(square2->m_index, square2));
    
    Square* square3 = m_pSquares_all->at(2 + m_squaresCount_horizontal * 2);
    square3->m_fillState = kSquareFillStateFilled;
    m_pSquares_filled->insert(std::make_pair(square3->m_index, square3));
    
    Square* square4 = m_pSquares_all->at(1 + m_squaresCount_horizontal * 2);
    square4->m_fillState = kSquareFillStateFilled;
    m_pSquares_filled->insert(std::make_pair(square4->m_index, square4));
    
    Square* square5 = m_pSquares_all->at(0 + m_squaresCount_horizontal);
    square5->m_fillState = kSquareFillStateFilled;
    m_pSquares_filled->insert(std::make_pair(square5->m_index, square5));
    
    return true;
}

void WyrezMap::setupSurroundingSquares()
{
    for (int i = 0; i < m_pSquares_all->size(); i++)
    {
        Square* pSquare = m_pSquares_all->at(i);
        int squarePosX = pSquare->origin.x/m_squareSide;
        int squarePosY = pSquare->origin.y/m_squareSide;
        
        /*
         ooo
         oxo
         ooo
         */
        if (         squarePosX > 0
                 &&  squarePosX < m_squaresCount_vertical-1
                 &&  squarePosY > 0
                 &&  squarePosY < m_squaresCount_horizontal-1)
        {
            pSquare->m_surroundingSquares->push_back(m_pSquares_all->at(    i - m_squaresCount_horizontal - 1   ));
            pSquare->m_surroundingSquares->push_back(m_pSquares_all->at(    i - m_squaresCount_horizontal       ));
            pSquare->m_surroundingSquares->push_back(m_pSquares_all->at(    i - m_squaresCount_horizontal + 1   ));
            pSquare->m_surroundingSquares->push_back(m_pSquares_all->at(    i - 1                               ));
            pSquare->m_surroundingSquares->push_back(m_pSquares_all->at(    i + 1                               ));
            pSquare->m_surroundingSquares->push_back(m_pSquares_all->at(    i + m_squaresCount_horizontal - 1   ));
            pSquare->m_surroundingSquares->push_back(m_pSquares_all->at(    i + m_squaresCount_horizontal       ));
            pSquare->m_surroundingSquares->push_back(m_pSquares_all->at(    i + m_squaresCount_horizontal + 1   ));
        }
        /*
         |oo
         |xo
         |oo
         */
        else if (    squarePosX == 0
                 &&  squarePosX < m_squaresCount_vertical-1
                 &&  squarePosY > 0
                 &&  squarePosY < m_squaresCount_horizontal-1)
        {
            pSquare->m_surroundingSquares->push_back(m_pSquares_all->at(    i - 1                               ));
            pSquare->m_surroundingSquares->push_back(m_pSquares_all->at(    i + 1                               ));
            pSquare->m_surroundingSquares->push_back(m_pSquares_all->at(    i + m_squaresCount_horizontal - 1   ));
            pSquare->m_surroundingSquares->push_back(m_pSquares_all->at(    i + m_squaresCount_horizontal       ));
            pSquare->m_surroundingSquares->push_back(m_pSquares_all->at(    i + m_squaresCount_horizontal + 1   ));
        }
        /*
         oo|
         ox|
         oo|
         */
        else if (    squarePosX > 0
                 &&  squarePosX == m_squaresCount_vertical-1
                 &&  squarePosY > 0
                 &&  squarePosY < m_squaresCount_horizontal-1)
        {
            pSquare->m_surroundingSquares->push_back(m_pSquares_all->at(    i - m_squaresCount_horizontal - 1   ));
            pSquare->m_surroundingSquares->push_back(m_pSquares_all->at(    i - m_squaresCount_horizontal       ));
            pSquare->m_surroundingSquares->push_back(m_pSquares_all->at(    i - m_squaresCount_horizontal + 1   ));
            pSquare->m_surroundingSquares->push_back(m_pSquares_all->at(    i - 1                               ));
            pSquare->m_surroundingSquares->push_back(m_pSquares_all->at(    i + 1                               ));
        }
        /*
         ooo
         oxo
         ---
         */
        else if (    squarePosX > 0
                 &&  squarePosX < m_squaresCount_vertical-1
                 &&  squarePosY == 0
                 &&  squarePosY < m_squaresCount_horizontal-1)
        {
            pSquare->m_surroundingSquares->push_back(m_pSquares_all->at(    i - m_squaresCount_horizontal       ));
            pSquare->m_surroundingSquares->push_back(m_pSquares_all->at(    i - m_squaresCount_horizontal + 1   ));
            pSquare->m_surroundingSquares->push_back(m_pSquares_all->at(    i + 1                               ));
            pSquare->m_surroundingSquares->push_back(m_pSquares_all->at(    i + m_squaresCount_horizontal       ));
            pSquare->m_surroundingSquares->push_back(m_pSquares_all->at(    i + m_squaresCount_horizontal + 1   ));
        }
        /*
         ---
         oxo
         ooo
         */
        else if (    squarePosX > 0
                 &&  squarePosX < m_squaresCount_vertical-1
                 &&  squarePosY > 0
                 &&  squarePosY == m_squaresCount_horizontal-1)
        {
            pSquare->m_surroundingSquares->push_back(m_pSquares_all->at(    i - m_squaresCount_horizontal - 1   ));
            pSquare->m_surroundingSquares->push_back(m_pSquares_all->at(    i - m_squaresCount_horizontal       ));
            pSquare->m_surroundingSquares->push_back(m_pSquares_all->at(    i - 1                               ));
            pSquare->m_surroundingSquares->push_back(m_pSquares_all->at(    i + m_squaresCount_horizontal - 1   ));
            pSquare->m_surroundingSquares->push_back(m_pSquares_all->at(    i + m_squaresCount_horizontal       ));
        }
        /*
         |oo
         |xo
         |--
         */
        else if (    squarePosX == 0
                 &&  squarePosX < m_squaresCount_vertical-1
                 &&  squarePosY == 0
                 &&  squarePosY < m_squaresCount_horizontal-1)
        {
            pSquare->m_surroundingSquares->push_back(m_pSquares_all->at(    i + 1                               ));
            pSquare->m_surroundingSquares->push_back(m_pSquares_all->at(    i + m_squaresCount_horizontal       ));
            pSquare->m_surroundingSquares->push_back(m_pSquares_all->at(    i + m_squaresCount_horizontal + 1   ));
        }
        /*
         oo|
         ox|
         --|
         */
        else if (    squarePosX > 0
                 &&  squarePosX == m_squaresCount_vertical-1
                 &&  squarePosY == 0
                 &&  squarePosY < m_squaresCount_horizontal-1)
        {
            pSquare->m_surroundingSquares->push_back(m_pSquares_all->at(    i - m_squaresCount_horizontal       ));
            pSquare->m_surroundingSquares->push_back(m_pSquares_all->at(    i - m_squaresCount_horizontal + 1   ));
            pSquare->m_surroundingSquares->push_back(m_pSquares_all->at(    i + 1                               ));
        }
        /*
         |--
         |xo
         |oo
         */
        else if (    squarePosX == 0
                 &&  squarePosX < m_squaresCount_vertical-1
                 &&  squarePosY > 0
                 &&  squarePosY == m_squaresCount_horizontal-1)
        {
            pSquare->m_surroundingSquares->push_back(m_pSquares_all->at(    i - 1                               ));
            pSquare->m_surroundingSquares->push_back(m_pSquares_all->at(    i + m_squaresCount_horizontal - 1   ));
            pSquare->m_surroundingSquares->push_back(m_pSquares_all->at(    i + m_squaresCount_horizontal       ));
        }
        /*
         --|
         ox|
         oo|
         */
        else if (    squarePosX > 0
                 &&  squarePosX == m_squaresCount_vertical-1
                 &&  squarePosY > 0
                 &&  squarePosY == m_squaresCount_horizontal-1)
        {
            pSquare->m_surroundingSquares->push_back(m_pSquares_all->at(    i - m_squaresCount_horizontal - 1   ));
            pSquare->m_surroundingSquares->push_back(m_pSquares_all->at(    i - m_squaresCount_horizontal       ));
            pSquare->m_surroundingSquares->push_back(m_pSquares_all->at(    i - 1                               ));
        }
    }
}

void WyrezMap::toggleFillForTouchLocation(CCPoint touchLocation)
{
    CCPoint squareLocation = ccp(floor(touchLocation.x/m_squareSide), floor(touchLocation.y/m_squareSide));
    int index = squareLocation.x * m_squaresCount_horizontal + squareLocation.y;
    Square* pSquare = m_pSquares_all->at(index);
    if (pSquare->m_fillState == kSquareFillStateEmpty) {
        pSquare->m_fillState = kSquareFillStateFilled;
        m_pSquares_filled->insert(std::make_pair(pSquare->m_index, pSquare));
    }
    else {
        pSquare->m_fillState = kSquareFillStateEmpty;
        std::map<int, Square*>::iterator itr = m_pSquares_filled->find(pSquare->m_index);
        m_pSquares_filled->erase(itr);
    }
    
}