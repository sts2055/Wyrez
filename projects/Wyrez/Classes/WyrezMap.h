//
//  WyrezMap.h
//  Wyrez
//
//  Created by stefan on 6/24/13.
//
//

#ifndef __Wyrez__WyrezMap__
#define __Wyrez__WyrezMap__

#include "cocos2d.h"
#include "Square.h"

USING_NS_CC;

#define kSquareSide 50
#define kDefaultSquaresVertical 100
#define kDefaultSquaresHorizontal 100


class WyrezMap
{
private:
    int m_squareSide;
    int m_squaresCount_vertical;
    int m_squaresCount_horizontal;
    int m_squaresCount_total;
    
    CCSize m_contentSize;
    
public:
    std::vector<CCPoint*> * m_pGridOrigins_vertical;
    std::vector<CCPoint*> * m_pGridOrigins_horizontal;
    std::vector<Square*> * m_pSquares;
    
public:
    WyrezMap();
    virtual ~WyrezMap();
    virtual bool init();
    void setupSurroundingSquares();
    void toggleFillForTouchLocation(CCPoint touchLocation);
    
    const int getSquaresCountVertical() {return m_squaresCount_vertical;}
    const int getSquaresCountHorizontal() {return m_squaresCount_horizontal;}
    const int getSquaresCountTotal() {return m_squaresCount_total;}
    const CCSize getContentSize() {return m_contentSize;}
    //std::vector<CCPoint*>* getGridOriginsVertical() {return m_pGridOrigins_vertical;}
    //std::vector<CCPoint*>* getGridOriginsHorizontal() {return m_pGridOrigins_horizontal;}
    //std::vector<Square*>* getSquares() {return m_pSquares;}
    
};

#endif /* defined(__Wyrez__WyrezMap__) */
