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
#include "WyrezMap.h"

USING_NS_CC;
USING_NS_CC_EXT;


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

static const ccColor4F kCOLOR_BLACK = {0.0, 0.0, 0.0, 1.0};
static const ccColor4F kCOLOR_GRAY_06 = {0.301, 0.301, 0.301, 1.0}; // grid default
static const ccColor4F kCOLOR_ORANGE = {1.0, 0.576, 0.141, 1.0}; // filled default
static const ccColor4F kCOLOR_BLUE = {0.050, 0.6, 0.988, 1.0}; // charged default
static const ccColor4F kCOLOR_WHITE = {1.0, 1.0, 1.0, 1.0}; // discharging default


class GameScene;
class GameLayer;


/**********************************************
 * header of GameHud
 **********************************************/
class GameHud : public CCLayer
{
private:
    const GameScene& m_rParentScene;
    
    
private:
    explicit GameHud(const GameScene& parent);
    GameHud() = delete; // no default constructor
    GameHud(const GameHud&) = delete; // cannot be copied
    GameHud& operator=(const GameHud&) = delete; // cannot be copied
    virtual bool init();
    
public:
    virtual ~GameHud();
    static GameHud* createWithParentScene(const GameScene& rParentScene);
};


/**********************************************
 * header of GameLayer
 **********************************************/
class GameLayer : public CCLayer
{
private:
    const GameScene& m_rParentScene;
    WyrezMap& m_rWyrezMap;
    bool m_wasScrolling;
    bool m_wasZooming;
    
private:
    GameLayer() = delete; // no default constructor
    GameLayer(const GameLayer&) = delete; // cannot be copied
    GameLayer& operator=(const GameLayer&) = delete; // cannot be copied
    
public:
    explicit GameLayer(const GameScene& rParent, WyrezMap& rWyrezMap);
    virtual ~GameLayer();
    virtual bool init();
    static GameLayer* createWithSceneAndMap(const GameScene& rParentScene,
                                            WyrezMap& rWyrezMap);
    
    virtual void registerWithTouchDispatcher();
    virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
    virtual void ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent);
    virtual void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent);
    virtual void ccTouchCancelled(CCTouch *pTouch, CCEvent *pEvent);
    
    void setWasZooming(bool b) {m_wasZooming = b;}
};


/**********************************************
 * header of GameDrawNode
 **********************************************/
class GameDrawNode : public CCDrawNode
{
private:
    const GameScene& m_rParentScene;
    WyrezMap& m_rWyrezMap;
    
    int m_squareSide;
    float m_scale;
    ccColor4F m_squareFillColor;
    ccColor4F m_squareChargedColor;
    ccColor4F m_squareDischargingColor;
    
private:
    GameDrawNode(const GameScene& rParentScene, WyrezMap& rWyrezMap);
    virtual bool init();
    
public:
    virtual ~GameDrawNode();
    static GameDrawNode* createWithSceneAndMap(const GameScene& rParentScene,
                                                          WyrezMap& rWyrezMap);
    
    void redraw(CCScrollView* view);
    void rearrange(CCScrollView* view);
    
    int getSquareSide() {return m_squareSide;}
    
};


/**********************************************
 * header of GameScene
 **********************************************/
class GameScene : public CCScene, public CCScrollViewDelegate
{
private:
    CCPoint m_visibleOrigin;
    CCSize m_visibleSize;
    
    WyrezMap* m_pWyrezMap;
    GameLayer* m_pGameLayer;
    CCScrollView* m_pScrollView;
    GameDrawNode* m_pDraw;
    GameHud* m_pGameHud;
    
private:    
    GameScene();
    virtual ~GameScene();
    virtual bool init();
    void gameLogic();
    
public: // functions
    CREATE_QUICK_SPRITE_FUNCTION;
    CREATE_FUNC(GameScene);
    static GameScene* scene();
    
    virtual void scrollViewDidScroll(CCScrollView* pView);
    virtual void scrollViewDidZoom(CCScrollView* pView);
    
    const CCPoint getVisibleOrigin() const {return m_visibleOrigin;}
    const CCSize getVisibleSize() const {return m_visibleSize;}
    const GameLayer& getGameLayer() const {return *m_pGameLayer;}
};


#endif /* defined(__Wyrez__GameLayer__) */
