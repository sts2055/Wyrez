//
//  GameLayer.cpp
//  Wyrez
//
//  Created by stefan on 6/20/13.
//
//

#include "GameLayer.h"


/*****************************************************************************************
 * implementation of inner class Hud
 *****************************************************************************************/
GameLayer::Hud::Hud(const GameLayer& parent)
: m_rParent(parent)
{
}

GameLayer::Hud::~Hud() // Destructor
{
    CCLOG("cocos2d: deallocing Hud %p", this);
    
    
}

bool GameLayer::Hud::init()
{
    if ( !CCLayer::init() )
    {
        return false;
    }

    return true;
}





/*****************************************************************************************
 * implementation of GameScene
 *****************************************************************************************/
GameLayer::GameLayer()
{
}

GameLayer::~GameLayer()
{
    CCLOG("cocos2d: deallocing GameLayer %p", this);
    
    for (auto point : *m_pGridOrigins_vertical)
    {
        delete point;
    }
    delete m_pGridOrigins_horizontal;
    
    for (auto point : *m_pGridOrigins_vertical)
    {
        delete point;
    }
    delete m_pGridOrigins_horizontal;
}

bool GameLayer::init()
{
    if ( !CCLayer::init() )
    {
        return false;
    }
    
    this->setTouchEnabled(true);
    
    m_visibleOrigin = CCDirector::sharedDirector()->getVisibleOrigin();
    m_visibleSize = CCDirector::sharedDirector()->getVisibleSize();
    
    m_pDraw = CCDrawNode::create();
    addChild(m_pDraw, 10);
    
    m_pGridOrigins_horizontal = new std::vector<CCPoint*>();
    m_pGridOrigins_vertical = new std::vector<CCPoint*>();
    
    
    
    CCLayer* layer = CCLayer::create();
    layer->setContentSize(CCSizeMake(kSquareSide * 1000, kSquareSide * 1000));
    auto scrollView = CCScrollView::create(m_visibleSize, layer);
    scrollView->setBounceable(false);
    scrollView->setDelegate(this);
    
    
    int horizontalLines = layer->getContentSize().height/kSquareSide;
    int verticalLines = layer->getContentSize().width/kSquareSide;
    
    CCLOG("horizontalLines %i", horizontalLines);
    CCLOG("verticalLines %i", verticalLines);
    
    for (int i = 0; i < horizontalLines; i++)
    {
        m_pGridOrigins_horizontal->push_back(new CCPoint(0, i * kSquareSide));
    }
    
    for (int i = 0; i < verticalLines; i++)
    {
        m_pGridOrigins_vertical->push_back(new CCPoint(i * kSquareSide, 0));
    }
    
    
    CCSprite *square = createQuickSprite(CCSizeMake(m_visibleSize.width, m_visibleSize.height), kCOLOR_GRAY_07);
    square->setAnchorPoint(CCPointZero);
    square->setPosition(CCPointZero);
    this->addChild(square);
    
    this->addChild(scrollView);
    this->addChild(Hud::createWithParent(*this));
    
    return true;
}

CCScene* GameLayer::scene()
{
    CCScene* scene = CCScene::create();
    
    GameLayer* gameLayer = GameLayer::create();
    scene->addChild(gameLayer);
    
    return scene;
}

void GameLayer::scrollViewDidScroll(CCScrollView* view)
{
    CCPoint offset = view->getContentOffset();
    CCPoint temp = ccp(offset.x/kSquareSide, offset.y/kSquareSide);
    CCLOG("temp x:%f  y:%f", temp.x, temp.y );
    
    // initially the view's offset is negative, so we make it positive to be able to create a rect
    // that detect which points are on screen
    CCRect rect = CCRectMake(-offset.x, -offset.y, m_visibleSize.width, m_visibleSize.height);
    m_pDraw->clear();
    
    for (auto pPoint : *m_pGridOrigins_horizontal)
    {
        if (pPoint->y >= rect.origin.y
            && pPoint->y <= rect.origin.y + m_visibleSize.height) {
            CCPoint fromPoint = CCPointMake(0, pPoint->y + offset.y);
            CCPoint toPoint = CCPointMake(m_visibleSize.width, fromPoint.y);
            m_pDraw->drawSegment(fromPoint, toPoint, 0.5f, ccc4f(0, 1, 0, 1));
        }
    }
    
    for (auto pPoint : *m_pGridOrigins_vertical)
    {
        if (pPoint->x >= rect.origin.x
            && pPoint->x <= rect.origin.x + m_visibleSize.width) {
            CCPoint fromPoint = CCPointMake(pPoint->x + offset.x, 0);
            CCPoint toPoint = CCPointMake(fromPoint.x, m_visibleSize.height);
            m_pDraw->drawSegment(fromPoint, toPoint, 0.5f, ccc4f(0, 1, 0, 1));
        }
    }
}

void GameLayer::scrollViewDidZoom(CCScrollView* view)
{
}
