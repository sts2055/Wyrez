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
#define kDefaultSquaresVertical 1000
#define kDefaultSquaresHorizontal 1000

static const ccColor3B kDEFAULT_COLOR_BACKGROUND    = {63,  63, 63}; // dark gray
static const ccColor3B kDEFAULT_COLOR_GRIDLINES     = {99,  99, 99}; // lighter gray
static const ccColor3B kDEFAULT_COLOR_FILL          = {255, 147,36}; // orange
static const ccColor3B kDEFAULT_COLOR_CHARGED       = {13,  153,252}; // light blue
static const ccColor3B kDEFAULT_COLOR_DISCHARGING   = {255, 255,255}; // white

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
    std::map<int, Square*> * m_pSquares_filled; // square's index as key
    
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
    
    // DEBUGGING
    typedef unsigned long long timestamp_t;
    static timestamp_t get_timestamp () {
        struct timeval now;
        gettimeofday (&now, NULL);
        return  now.tv_usec + (timestamp_t)now.tv_sec * 1000000;
    }
    
    static void printFileAtLocation(std::string s) {
        FILE *in;
        char c;
        in = fopen(s.c_str(), "r");
        if(in != NULL)
        {
            while((c = fgetc(in)) != EOF)
                putchar(c);
            
            fclose(in);
        }
    }
    
};

#endif /* defined(__Wyrez__WyrezMap__) */
