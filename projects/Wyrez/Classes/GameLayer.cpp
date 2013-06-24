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
: m_visibleOrigin(CCPointZero)
, m_visibleSize(CCSizeZero)
, m_squareSide(0)
, m_scale(0)
, m_squaresCount(0)
, m_squareFillColor(kCOLOR_BLACK)
, m_squareChargedColor(kCOLOR_BLACK)
, m_squareDischargingColor(kCOLOR_BLACK)
, m_pScrollView(nullptr)
, m_pDraw(nullptr)
, m_pGridOrigins_vertical(nullptr)
, m_pGridOrigins_horizontal(nullptr)
{
}

GameLayer::~GameLayer()
{
    CCLOG("cocos2d: deallocing GameLayer %p", this);
    
    for (auto pPoint : *m_pGridOrigins_vertical)
    {
        delete pPoint;
    }
    delete m_pGridOrigins_horizontal;
    
    for (auto pPoint : *m_pGridOrigins_vertical)
    {
        delete pPoint;
    }
    delete m_pGridOrigins_horizontal;
    
    for (auto square : *m_pSquares)
    {
        delete square;
    }
    delete m_pSquares;
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
    m_squareSide = kSquareSide;
    m_scale = kDefaultScale;
    m_squaresCount = kSquareSide * kSquareSide;
    
    m_pDraw = CCDrawNode::create();
    addChild(m_pDraw, 10);
    
    m_pGridOrigins_horizontal = new std::vector<CCPoint*>();
    m_pGridOrigins_vertical = new std::vector<CCPoint*>();
    m_pSquares = new std::vector<Square*>();
    m_squareFillColor = kCOLOR_ORANGE;
    m_squareChargedColor = kCOLOR_BLUE;
    m_squareDischargingColor = kCOLOR_WHITE;
    
    
    CCLayer* layer = CCLayer::create();
    layer->setContentSize(CCSizeMake(m_squareSide * kSquareXCount, m_squareSide * kSquareYCount));
    auto scrollView = CCScrollView::create(m_visibleSize, layer);
    scrollView->setBounceable(false);
    scrollView->setFMinScale(0.25f);
    scrollView->setFMaxScale(1.0f);
    scrollView->setDelegate(this);
    m_pScrollView = scrollView;
    
    int verticalLines = layer->getContentSize().width/m_squareSide;
    int horizontalLines = layer->getContentSize().height/m_squareSide;
    
    CCLOG("verticalLines %i", verticalLines);
    CCLOG("horizontalLines %i", horizontalLines);
    
    
    for (int i = 0; i < verticalLines; i++)
    {
        m_pGridOrigins_vertical->push_back(new CCPoint(i * m_squareSide, 0));
    }
    
    for (int i = 0; i < horizontalLines; i++)
    {
        m_pGridOrigins_horizontal->push_back(new CCPoint(0, i * m_squareSide));
    }
    
    
    for (int i = 0; i < verticalLines; i++)
    {
        for (int j = 0; j < horizontalLines; j++)
        {
            m_pSquares->push_back(new Square(i * m_squareSide, j * m_squareSide, m_squareSide, m_squareSide));
        }
    }
    
    /*for (auto cIter = m_pSquares->begin();
         cIter != m_pSquares->end(); ++cIter)
    {
        //std::cout << "square x:" << (*cIter)->origin.x << " y:" << (*cIter)->origin.y << " width:" << (*cIter)->size.width << " height:" << (*cIter)->size.height << "\n";
    }*/
    
    
    CCSprite *background = createQuickSprite(CCSizeMake(m_visibleSize.width, m_visibleSize.height), ccc3(51, 51, 51));
    background->setAnchorPoint(CCPointZero);
    background->setPosition(CCPointZero);
    this->addChild(background);
    
    this->addChild(scrollView);
    this->addChild(Hud::createWithParent(*this));
    
    // Setup the grid
    this->schedule(schedule_selector(GameLayer::gameLogic), 0.1);
    this->scrollViewDidScroll(scrollView);
    
    return true;
}

CCScene* GameLayer::scene()
{
    CCScene* scene = CCScene::create();
    
    GameLayer* gameLayer = GameLayer::create();
    scene->addChild(gameLayer);
    
    return scene;
}

void GameLayer::gameLogic()
{
    if (m_pScrollView->isDragging()
        || m_pScrollView->isZooming()) {
        return;
    }
    this->redraw(m_pScrollView);
}

void GameLayer::redraw(CCScrollView* pView)
{
    m_pDraw->clear();
    
    // the view's offset is negative
    CCPoint offset = pView->getContentOffset();
    
    // - calculate the horizontal and vertical start indexes for the vector to only iterate over the points that are on screen
    // - MIN because h or vIndex can be above the max index of the vector when zooming in/out at the very left/top edge
    // - Right now it shows either no h or v grid when at the very edge and zooming in/out as it displays areas that are out of bounds
    // - This could be prevented by for example adding more gridOrigins to the vectors
    int vIndex = MIN(floor(-offset.x/m_squareSide), kSquareXCount);
    int hIndex = MIN(floor(-offset.y/m_squareSide), kSquareYCount);
    
    int vLinesPerScreen = m_visibleSize.width/m_squareSide + 2;
    int hLinesPerScreen = m_visibleSize.height/m_squareSide + 2;
    
    // - MIN because h or vIndex_max can be above the max index of the vector when scrolling to the very left/top edge
    int vIndex_max = MIN(vIndex + vLinesPerScreen, kSquareXCount) ;
    int hIndex_max = MIN(hIndex + hLinesPerScreen, kSquareYCount);
    
    
    //int k = 0;
    for (int i = vIndex; i < vIndex_max; i++)
    {
        for (int j = i * kSquareXCount + hIndex; j < i * kSquareXCount + hIndex + hLinesPerScreen; j++)
        {
            if (j >= m_squaresCount) {
                break;
            }
            
            Square *square = m_pSquares->at(j);
            
            if (square->m_fillState == kSquareFillStateEmpty) {
                continue;
            }
            
            CCPoint points[] = {
                CCPoint(square->origin.x + offset.x,                  square->origin.y + offset.y),
                CCPoint(square->origin.x + offset.x + m_squareSide,   square->origin.y + offset.y),
                CCPoint(square->origin.x + offset.x + m_squareSide,   square->origin.y + offset.y + m_squareSide),
                CCPoint(square->origin.x + offset.x,                  square->origin.y + offset.y + m_squareSide)
            };
            
            switch (square->m_chargeState) {
                case kSquareChargeStateNoCharge:
                    m_pDraw->drawPolygon(points, sizeof(points)/sizeof(points[0]), kCOLOR_BLACK, 0, kCOLOR_ORANGE);
                    break;
                case kSquareChargeStateCharged:
                    m_pDraw->drawPolygon(points, sizeof(points)/sizeof(points[0]), kCOLOR_BLACK, 0, kCOLOR_BLUE);
                    break;
                case kSquareChargeStateDischarging:
                    m_pDraw->drawPolygon(points, sizeof(points)/sizeof(points[0]), kCOLOR_BLACK, 0, kCOLOR_WHITE);
                    break;
            }
            
            //k++;
        }
    }
    //std::cout << "k:" << k << "\n";
    
    // m_squareSide being smaller than half of kSquareSide means that the map scale is smaller than 0.5
    // at which point we don't want to display a grid at all
    if (m_squareSide < kSquareSide/2) {
        return;
    }
    
    for (auto cIter = m_pGridOrigins_vertical->cbegin() + vIndex;
         cIter != m_pGridOrigins_vertical->cbegin() + vIndex_max; ++cIter)
    {
        CCPoint fromPoint = CCPointMake((*cIter)->x + offset.x, 0);
        CCPoint toPoint = CCPointMake(fromPoint.x, m_visibleSize.height);
        m_pDraw->drawSegment(fromPoint, toPoint, 0.5f, kCOLOR_GRAY_06);
    }
    
    for (auto cIter = m_pGridOrigins_horizontal->cbegin() + hIndex;
         cIter != m_pGridOrigins_horizontal->cbegin() + hIndex_max; ++cIter)
    {
        CCPoint fromPoint = CCPointMake(0, (*cIter)->y + offset.y);
        CCPoint toPoint = CCPointMake(m_visibleSize.width, fromPoint.y);
        m_pDraw->drawSegment(fromPoint, toPoint, 0.5f, kCOLOR_GRAY_06);
    }
}

void GameLayer::rearrange(CCScrollView* pView)
{
    CCNode* container = pView->getContainer();
    m_squareSide = floor(kSquareSide * container->getScale());
    m_scale = container->getScale();
    
    int i = 0;
    for (auto cIter = m_pGridOrigins_vertical->begin();
         cIter != m_pGridOrigins_vertical->end(); ++cIter)
    {
        (*cIter)->x = i * m_squareSide;
        i++;
    }
    
    i = 0;
    for (auto cIter = m_pGridOrigins_horizontal->begin();
         cIter != m_pGridOrigins_horizontal->end(); ++cIter)
    {
        (*cIter)->y = i * m_squareSide;
        i++;
    }
    
    int k = 0;
    for (i = 0; i < m_pGridOrigins_vertical->size(); i++)
    {
        for (int j = 0; j < m_pGridOrigins_horizontal->size(); j++)
        {
            Square *square = m_pSquares->at(k);
            square->origin.x = i * m_squareSide;
            square->origin.y = j * m_squareSide;
            square->size.width = m_squareSide;
            square->size.height = m_squareSide;
            k++;
        }
    }
}

bool GameLayer::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
    std::cout << "ccTouchBegan:\n";
    return true;
}

void GameLayer::ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent)
{
    std::cout << "ccTouchMoved:\n";
}

void GameLayer::ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent)
{
    std::cout << "ccTouchEnded:\n";
}

void GameLayer::ccTouchCancelled(CCTouch *pTouch, CCEvent *pEvent)
{
    std::cout << "ccTouchCancelled:\n";
}

void GameLayer::scrollViewDidScroll(CCScrollView* view)
{
    //this->pauseSchedulerAndActions();
    this->redraw(view);
    //this->resumeSchedulerAndActions();
}

void GameLayer::scrollViewDidZoom(CCScrollView* view)
{
    //this->pauseSchedulerAndActions();
    this->rearrange(view);
    //this->resumeSchedulerAndActions();
}
