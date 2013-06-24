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

static const ccColor4F kCOLOR_BLACK = {0.0, 0.0, 0.0, 1.0};
static const ccColor4F kCOLOR_GRAY_06 = {0.301, 0.301, 0.301, 1.0}; // grid default
static const ccColor4F kCOLOR_ORANGE = {1.0, 0.576, 0.141, 1.0}; // filled default
static const ccColor4F kCOLOR_BLUE = {0.050, 0.6, 0.988, 1.0}; // charged default
static const ccColor4F kCOLOR_WHITE = {1.0, 1.0, 1.0, 1.0}; // discharging default

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
    int m_squaresCount;
    ccColor4F m_squareFillColor;
    ccColor4F m_squareChargedColor;
    ccColor4F m_squareDischargingColor;
    
    CCScrollView* m_pScrollView;
    CCDrawNode* m_pDraw;
    std::vector<CCPoint*> * m_pGridOrigins_vertical;
    std::vector<CCPoint*> * m_pGridOrigins_horizontal;
    std::vector<Square*> * m_pSquares;
    
    
public: // members
    
private: // functions
    GameLayer();
    virtual bool init();
    virtual ~GameLayer();
    void gameLogic();
    void redraw(CCScrollView* view);
    void rearrange(CCScrollView* view);
    
public: // functions
    CREATE_QUICK_SPRITE_FUNCTION;
    CREATE_FUNC(GameLayer);
    static CCScene* scene();
    
    virtual void scrollViewDidScroll(CCScrollView* pView);
    virtual void scrollViewDidZoom(CCScrollView* pView);
    
    virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
    virtual void ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent);
    virtual void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent);
    virtual void ccTouchCancelled(CCTouch *pTouch, CCEvent *pEvent);
};

#endif /* defined(__Wyrez__GameLayer__) */
