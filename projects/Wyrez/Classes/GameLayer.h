//
//  GameLayer.h
//  Wyrez
//
//  Created by stefan on 6/20/13.
//
//

#ifndef __Wyrez__GameLayer__
#define __Wyrez__GameLayer__

#include "cocos2d.h"
#include "cocos-ext.h"
#include "Square.h"

USING_NS_CC;
USING_NS_CC_EXT;

/**
 * define a create function for a specific type, such as CCLayer
 * @__TYPE__ class type to add create(x), such as CCLayer
 * @__PARENTTYPE__ type of the argument passed into the create function
 */
#define CREATE_FUNC_WITH_PARENT(__TYPE__,__PARENTTYPE__) \
static __TYPE__* createWithParent(const __PARENTTYPE__& parent) \
{ \
__TYPE__* pRet = new __TYPE__(parent); \
if (pRet && pRet->init()) \
{ \
pRet->autorelease(); \
return pRet; \
} \
else \
{ \
delete pRet; \
pRet = NULL; \
return NULL; \
} \
}

#define CREATE_QUICK_SPRITE_FUNCTION \
static CCSprite* createQuickSprite(CCSize size, ccColor3B color) \
{ \
CCSprite* sprite = new CCSprite(); \
sprite->initWithFile("blank.png"); \
sprite->setTextureRect(CCRectMake(0,0,size.width,size.height)); \
sprite->setColor(color); \
sprite->autorelease(); \
return sprite; \
}

#define kDefaultScale 1
#define kSquareSide 50
#define kSquareXCount 1000
#define kSquareYCount 1000


static const ccColor3B kCOLOR_GRAY_03 = {153, 153, 153};
static const ccColor3B kCOLOR_GRAY_05 = {102, 102, 102};
static const ccColor3B kCOLOR_GRAY_06 = {77, 77, 77};
static const ccColor3B kCOLOR_GRAY_07 = {51, 51, 51};
static const ccColor3B kCOLOR_PINK_02 = {255, 128, 229};


class GameLayer : public CCLayer, public CCScrollViewDelegate
{
private:
// Inner Class: Hud
    class Hud : public CCLayer
    {
    private: // members
        const GameLayer& m_rParent;
        
        
    private: // functions
        explicit Hud(const GameLayer & parent);
        Hud() = delete; // no default constructor
        Hud(const Hud&) = delete; // cannot be copied
        Hud& operator=(const Hud&) = delete; // cannot be copied
        virtual bool init();
        
    public: // functions
        virtual ~Hud();
        CREATE_FUNC_WITH_PARENT(Hud, GameLayer);
        
        
    };
    
// Class: GameLayer
private: // members
    CCPoint m_visibleOrigin;
    CCSize m_visibleSize;
    int m_squareSide;
    float m_scale;
    
    CCDrawNode* m_pDraw;
    std::vector<CCPoint*> * m_pGridOrigins_vertical;
    std::vector<CCPoint*> * m_pGridOrigins_horizontal;
    std::vector<Square*> * m_pSquares;
    
    
public: // members
    
private: // functions
    GameLayer();
    virtual bool init();
    virtual ~GameLayer();
    
public: // functions
    CREATE_QUICK_SPRITE_FUNCTION;
    CREATE_FUNC(GameLayer);
    static CCScene* scene();
    
    virtual void scrollViewDidScroll(CCScrollView* view);
    virtual void scrollViewDidZoom(CCScrollView* view);
};

#endif /* defined(__Wyrez__GameLayer__) */
