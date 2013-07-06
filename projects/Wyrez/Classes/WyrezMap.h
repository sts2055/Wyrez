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

static const int kDefaultSquareSide = 50;

class WyrezMap
{
private:
    int m_squareSide;
    int m_squaresCount_vertical;
    int m_squaresCount_horizontal;
    int m_squaresCount_total;
    
    ccColor3B m_backgroundColor;
    ccColor3B m_gridLinesColor;
    ccColor3B m_squareFillColor;
    ccColor3B m_squareChargedColor;
    ccColor3B m_squareDischargingColor;
    
    std::string m_mapName;
    
    CCSize m_contentSize;
    
    CCDictionary* m_writeToFileRootDict;
    
public:
    std::vector<CCPoint*> * m_pGridOrigins_vertical;
    std::vector<CCPoint*> * m_pGridOrigins_horizontal;
    std::vector<Square*> * m_pSquares_all;
    // square's index as key, holds pointers to Squares that will be deleted by m_pSquares_all
    std::map<int, Square*> * m_pSquares_filled; 
    
public:
    WyrezMap();
    virtual ~WyrezMap();
    virtual bool init();
    void setupSurroundingSquares();
    void toggleFillForTouchLocation(CCPoint touchLocation);
    void writeToFile(int step);
    
    void setSquareSide(int i){m_squareSide = i;}
    
    const int getSquareSide(){return m_squareSide;}
    const int getSquaresCountVertical() {return m_squaresCount_vertical;}
    const int getSquaresCountHorizontal() {return m_squaresCount_horizontal;}
    const int getSquaresCountTotal() {return m_squaresCount_total;}
    const CCSize getContentSize() {return m_contentSize;}
    
    ccColor3B getBackgroundColor() {return m_backgroundColor;}
    ccColor3B getGridLinesColor() {return m_gridLinesColor;}
    ccColor3B getSquareFillColor() {return m_squareFillColor;}
    ccColor3B getSquareChargedColor() {return m_squareChargedColor;}
    ccColor3B getSquareDischargingColor() {return m_squareDischargingColor;}
    
    
    void test(int step);
    
};


#endif /* defined(__Wyrez__WyrezMap__) */
