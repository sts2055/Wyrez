//
//  GameLayer.cpp
//  Wyrez
//
//  Created by stefan on 6/20/13.
//
//

#include "GameLayer.h"


/*****************************************************************************************
 * implementation of GameHud
 *****************************************************************************************/
GameHud::GameHud(const GameScene& rParentScene)
: m_rParentScene(rParentScene)
{
}

GameHud::~GameHud() // Destructor
{
    CCLOG("cocos2d: deallocing GameHud %p", this);
    
    
}

bool GameHud::init()
{
    if ( !CCLayer::init() )
    {
        return false;
    }

    return true;
}

GameHud* GameHud::createWithParentScene(const GameScene& rParentScene)
{
    GameHud* pRet = new GameHud(rParentScene);
    if (pRet && pRet->init())
    {
        pRet->autorelease();
        return pRet;
    }
    else
    {
        delete pRet;
        pRet = NULL;
        return NULL;
    }
}



/*****************************************************************************************
 * implementation of GameLayer
 *****************************************************************************************/
GameLayer::GameLayer(const GameScene& rParent)
: m_rParentScene(rParent)
{
}

GameLayer::~GameLayer()
{
    CCLOG("cocos2d: deallocing GameLayer %p", this);
    
    
}

bool GameLayer::init()
{
    if ( !CCLayer::init() )
    {
        return false;
    }
    
    this->setTouchEnabled(true);
    
    
    return true;
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

GameLayer* GameLayer::createWithParentScene(const GameScene& rParentScene)
{
    GameLayer* pRet = new GameLayer(rParentScene);
    if (pRet && pRet->init())
    {
        pRet->autorelease();
        return pRet;
    }
    else
    {
        delete pRet;
        pRet = NULL;
        return NULL;
    }
}




/*****************************************************************************************
 * implementation of GameDrawNode
 *****************************************************************************************/
GameDrawNode::GameDrawNode(const GameScene& rParentScene)
: m_rParentScene(rParentScene)
, m_squareSide(0)
, m_scale(0)
, m_squaresCount(0)
, m_squareFillColor(kCOLOR_BLACK)
, m_squareChargedColor(kCOLOR_BLACK)
, m_squareDischargingColor(kCOLOR_BLACK)
, m_pGridOrigins_vertical(nullptr)
, m_pGridOrigins_horizontal(nullptr)
{
    
}

GameDrawNode::~GameDrawNode()
{
    CCLOG("cocos2d: deallocing GameDrawNode %p", this);
    
    for (auto pPoint : *m_pGridOrigins_vertical)
    {
        delete pPoint;
    }
    delete m_pGridOrigins_vertical;
    
    for (auto pPoint : *m_pGridOrigins_horizontal)
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

bool GameDrawNode::initWithMap(WyrezMap* pWyrezMap)
{
    if ( !CCDrawNode::init() )
    {
        return false;
    }
    
    if (pWyrezMap == nullptr) {
        CCLOG("pWyrezMap is null");
        return false;
    }
    
    m_squareSide = kSquareSide;
    m_scale = kDefaultScale;
    m_squaresCount = pWyrezMap->getSquaresCountVertical() * pWyrezMap->getSquaresCountHorizontal();
    
    m_pGridOrigins_vertical = new std::vector<CCPoint*>();
    for (auto iter : *pWyrezMap->getGridOriginsVertical())
    {
        m_pGridOrigins_vertical->push_back(new CCPoint(*iter));
    }
    
    m_pGridOrigins_horizontal = new std::vector<CCPoint*>();
    for (auto iter : *pWyrezMap->getGridOriginsHorizontal())
    {
        m_pGridOrigins_horizontal->push_back(new CCPoint(*iter));
    }
    
    m_pSquares = new std::vector<Square*>();
    for (auto iter : *pWyrezMap->getSquares())
    {
        m_pSquares->push_back(new Square(*iter));
    }
    
    m_squareFillColor = kCOLOR_ORANGE;
    m_squareChargedColor = kCOLOR_BLUE;
    m_squareDischargingColor = kCOLOR_WHITE;
    
    CCLOG("m_pGridOrigins_vertical->size %li", m_pGridOrigins_vertical->size());
    CCLOG("m_pGridOrigins_horizontal->size %li", m_pGridOrigins_horizontal->size());
    CCLOG("m_pSquares->size %li", m_pSquares->size());

    delete pWyrezMap;
    
    return true;
}

GameDrawNode* GameDrawNode::createWithSceneAndMap(const GameScene& rParentScene, WyrezMap* pWyrezMap)
{
    GameDrawNode* pRet = new GameDrawNode(rParentScene);
    if (pRet && pRet->initWithMap(pWyrezMap))
    {
        pRet->autorelease();
        return pRet;
    }
    else
    {
        delete pRet;
        pRet = NULL;
        return NULL;
    }
}

void GameDrawNode::redraw(CCScrollView* pView)
{
    this->clear();
    
    // the view's offset is negative
    CCPoint offset = pView->getContentOffset();
    
    // - calculate the horizontal and vertical start indexes for the vector to only iterate over the points that are on screen
    // - MIN because h or vIndex can be above the max index of the vector when zooming in/out at the very left/top edge
    // - Right now it shows either no h or v grid when at the very edge and zooming in/out as it displays areas that are out of bounds
    // - This could be prevented by for example adding more gridOrigins to the vectors
    int vIndex = MIN(floor(-offset.x/m_squareSide), kSquareXCount);
    int hIndex = MIN(floor(-offset.y/m_squareSide), kSquareYCount);
    
    int vLinesPerScreen = m_rParentScene.getVisibleSize().width/m_squareSide + 2;
    int hLinesPerScreen = m_rParentScene.getVisibleSize().height/m_squareSide + 2;
    
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
                    this->drawPolygon(points, sizeof(points)/sizeof(points[0]), kCOLOR_BLACK, 0, kCOLOR_ORANGE);
                    break;
                case kSquareChargeStateCharged:
                    this->drawPolygon(points, sizeof(points)/sizeof(points[0]), kCOLOR_BLACK, 0, kCOLOR_BLUE);
                    break;
                case kSquareChargeStateDischarging:
                    this->drawPolygon(points, sizeof(points)/sizeof(points[0]), kCOLOR_BLACK, 0, kCOLOR_WHITE);
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
        CCPoint toPoint = CCPointMake(fromPoint.x, m_rParentScene.getVisibleSize().height);
        this->drawSegment(fromPoint, toPoint, 0.5f, kCOLOR_GRAY_06);
    }
    
    for (auto cIter = m_pGridOrigins_horizontal->cbegin() + hIndex;
         cIter != m_pGridOrigins_horizontal->cbegin() + hIndex_max; ++cIter)
    {
        CCPoint fromPoint = CCPointMake(0, (*cIter)->y + offset.y);
        CCPoint toPoint = CCPointMake(m_rParentScene.getVisibleSize().width, fromPoint.y);
        this->drawSegment(fromPoint, toPoint, 0.5f, kCOLOR_GRAY_06);
    }
}

void GameDrawNode::rearrange(CCScrollView* pView)
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




/*****************************************************************************************
 * implementation of GameScene
 *****************************************************************************************/

GameScene::GameScene()
: m_visibleOrigin(CCPointZero)
, m_visibleSize(CCSizeZero)
, m_pGameLayer(nullptr)
, m_pScrollView(nullptr)
, m_pDraw(nullptr)
{
    
}

GameScene::~GameScene()
{
    CCLOG("cocos2d: deallocing GameScene %p", this);
}

bool GameScene::init()
{
    if ( !CCScene::init() )
    {
        return false;
    }
    
    m_visibleOrigin = CCDirector::sharedDirector()->getVisibleOrigin();
    m_visibleSize = CCDirector::sharedDirector()->getVisibleSize();
    
    CCSprite *background = createQuickSprite(CCSizeMake(m_visibleSize.width, m_visibleSize.height), ccc3(51, 51, 51));
    background->setAnchorPoint(CCPointZero);
    background->setPosition(CCPointZero);
    
    WyrezMap* pWyresMap = new WyrezMap();
    pWyresMap->init();
    
    m_pDraw = GameDrawNode::createWithSceneAndMap(*this, pWyresMap);
    
    m_pGameLayer = GameLayer::createWithParentScene(*this);
    m_pGameLayer->setContentSize(pWyresMap->getContentSize());
    
    m_pScrollView = CCScrollView::create(m_visibleSize, m_pGameLayer);
    m_pScrollView->setBounceable(false);
    m_pScrollView->setFMinScale(0.25f);
    m_pScrollView->setFMaxScale(1.0f);
    m_pScrollView->setDelegate(this);
    
    m_pGameHud = GameHud::createWithParentScene(*this);
    
    this->addChild(background);
    this->addChild(m_pScrollView);
    this->addChild(m_pDraw);
    this->addChild(m_pGameHud);
    
    
    this->schedule(schedule_selector(GameScene::gameLogic), 0.1);

    return true;
}

GameScene* GameScene::scene()
{
    return GameScene::create();
}

void GameScene::gameLogic()
{
    if (m_pScrollView->isDragging()
        || m_pScrollView->isZooming()) {
        return;
    }
    m_pDraw->redraw(m_pScrollView);
}

void GameScene::scrollViewDidScroll(CCScrollView* view)
{
    //this->pauseSchedulerAndActions();
    m_pDraw->redraw(view);
    //this->resumeSchedulerAndActions();
}

void GameScene::scrollViewDidZoom(CCScrollView* view)
{
    //this->pauseSchedulerAndActions();
    m_pDraw->rearrange(view);
    //this->resumeSchedulerAndActions();
}















