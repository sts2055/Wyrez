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
#include "BrushMenuItemSprite.h"
#include "ResourceProgressTimer.h"

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
static const float kGameLogicIntervalMax = 0.05;
static const float kGameLogicIntervalMin = 0.85;
static const float kGameLogicIntervalEpsilon = 0.01;
static const ccColor4F kCOLOR_BLACK = {0.0, 0.0, 0.0, 1.0};



class GameScene;
class GameLayer;

/**********************************************
 * header of GameScrollView
 **********************************************/
class GameScrollView : public CCScrollView
{
private:
    bool m_shouldScroll;
    
private:
    GameScrollView();
    
public:
    virtual ~GameScrollView();
    
    
    void setFMaxScale(float f) { m_fMaxScale = f; };
    void setFMinScale(float f) { m_fMinScale = f; };
    
    static GameScrollView* create(CCSize size, CCNode* container = NULL);
    
    virtual void ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent);
    
    void setShouldScroll(bool b) {m_shouldScroll = b;}
};



/**********************************************
 * header of GameHud
 **********************************************/
typedef enum {
    kSqDtlOrient_right_top,
    kSqDtlOrient_right_bottom,
    kSqDtlOrient_left_top,
    kSqDtlOrient_left_bottom
} SqDtlOrientTypes;

typedef enum {
    kButtonPlay
} ButtonTypes;

class GameHud : public CCLayer
, public BrushMenuItemSpriteDelegate
, public ResourceProgressTimerDelegate
{
private:
    GameScene& m_rParentScene;
    WyrezMap& m_rWyrezMap;
    CCMenu* m_pActive_menu_primary;
    CCMenu* m_pActive_menu_secondary;
    ResourceProgressTimer* m_progressBar;
    
private:
    explicit GameHud(GameScene& parent, WyrezMap& rWyrezMap);
    GameHud() = delete;
    virtual bool init();
    
    CCSprite* createCustomSprite(std::string name, ccColor3B color);
    
    template<class T = CCMenuItemSprite>
    T* createMenuItemSpriteWithIcon(std::string icon, SEL_MenuHandler selector);
    
    void loadMenuPrimary_displayMode();
    void loadMenuPrimary_buildMode();
    
    void togglePlayPause();
    void accelerate();
    void decelerate();
    void uploadMap();
    void saveMap();
    void doNothing() {}; // used for brush mode
    virtual void brushMenuItemSpriteIsSelected();
    virtual void brushMenuItemSpriteIsUnselected();
    virtual void updateResourceHandler(int step);
    virtual void resourceHandlerFinished();
    
public:
    virtual ~GameHud();
    static GameHud* createWithParentScene(GameScene& rParentScene, WyrezMap& rWyrezMap);
    
    void loadMenuSecondary_squareDetail(Square& rSquare, CCPoint worldLocation);
};


/**********************************************
 * header of GameLayer
 **********************************************/
class GameLayer : public CCLayer
{
private:
    GameScene& m_rParentScene;
    WyrezMap& m_rWyrezMap;
    bool m_wasScrolling;
    bool m_wasZooming;
    bool m_brushModeActive;
    Square* m_brushModeCurrentSquare;
    timeval m_touchTime;
    CCPoint m_touchLocationCurrent;
    bool m_wasManipulatingSquare;
    
private:
    GameLayer() = delete;
    void touchDurationListener();
    void manipulateSquare(Square& rSquare);
    
public:
    explicit GameLayer(GameScene& rParent, WyrezMap& rWyrezMap);
    virtual ~GameLayer();
    virtual bool init();
    static GameLayer* createWithSceneAndMap(GameScene& rParentScene,
                                            WyrezMap& rWyrezMap);
    
    virtual void registerWithTouchDispatcher();
    virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
    virtual void ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent);
    virtual void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent);
    virtual void ccTouchCancelled(CCTouch *pTouch, CCEvent *pEvent);
    
    void setWasZooming(bool b) {m_wasZooming = b;}
    void setBrushModeActive(bool b) {m_brushModeActive = b;}
    
    static timeval get_timestamp () {
        struct timeval now;
        gettimeofday (&now, NULL);
        return  now;
    }
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
    
    ccColor4F m_gridLinesColor;
    ccColor4F m_squareFillColor;
    ccColor4F m_squareChargedColor;
    ccColor4F m_squareDischargingColor;
    
private:
    explicit GameDrawNode(const GameScene& rParentScene, WyrezMap& rWyrezMap);
    GameDrawNode() = delete;
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
    WyrezMap* m_pWyrezMap;
    GameLayer* m_pGameLayer;
    GameScrollView* m_pScrollView;
    GameDrawNode* m_pDraw;
    GameHud* m_pGameHud;
    float m_gameLogicInterval;
    
public:
    CCPoint m_visibleOrigin;
    CCSize m_visibleSize;
    
private:    
    GameScene();
    virtual ~GameScene();
    virtual bool init();
    bool isAlmostEqual(float x, float y);
    void gameLogic();
    void handleCharges();
    
    
    
public: // functions
    CREATE_QUICK_SPRITE_FUNCTION;
    CREATE_FUNC(GameScene);
    static GameScene* scene() { return GameScene::create(); }
    
    void redrawDrawNode();
    void disableScrolling();
    void enableScrolling();
    void openSquareDetailMenu(Square& rSquare, CCPoint worldLocation);
    void decelerateGameLogic();
    void accelerateGameLogic();
    virtual void scrollViewDidScroll(CCScrollView* pView);
    virtual void scrollViewDidZoom(CCScrollView* pView);
    
    void pause() { this->pauseSchedulerAndActions(); };
    void unpause() { this->resumeSchedulerAndActions(); }
    bool isPaused() { return this->getScheduler()->isTargetPaused(this); }
};


#endif /* defined(__Wyrez__GameLayer__) */
