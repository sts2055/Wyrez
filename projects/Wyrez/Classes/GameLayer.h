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
class GameHud : public CCLayer, public BrushMenuItemSpriteDelegate
{
private:
    GameScene& m_rParentScene;
    WyrezMap& m_rWyrezMap;
    CCMenu* m_active_menu;
    
private:
    explicit GameHud(GameScene& parent, WyrezMap& rWyrezMap);
    GameHud() = delete;
    virtual bool init();
    
    CCSprite* createCustomSprite(std::string name, ccColor3B color);
    CCMenuItemSprite* createMenuItemSpriteWithIcon(std::string icon, SEL_MenuHandler selector);
    void loadMenu_displayMode();
    void loadMenu_buildMode();
    void doNothing() {}; // used for brush mode
    
    virtual void brushMenuItemSpriteIsSelected();
    virtual void brushMenuItemSpriteIsUnselected();
    
public:
    virtual ~GameHud();
    static GameHud* createWithParentScene(GameScene& rParentScene, WyrezMap& rWyrezMap);
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
    
private:
    GameLayer() = delete;
    
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
    
public:
    CCPoint m_visibleOrigin;
    CCSize m_visibleSize;
    
private:    
    GameScene();
    virtual ~GameScene();
    virtual bool init();
    void gameLogic();
    void handleCharges();
    
public: // functions
    CREATE_QUICK_SPRITE_FUNCTION;
    CREATE_FUNC(GameScene);
    static GameScene* scene();
    
    void redrawDrawNode();
    void disableScrolling();
    void enableScrolling();
    virtual void scrollViewDidScroll(CCScrollView* pView);
    virtual void scrollViewDidZoom(CCScrollView* pView);
    const GameLayer& getGameLayer() const {return *m_pGameLayer;}
};


#endif /* defined(__Wyrez__GameLayer__) */
