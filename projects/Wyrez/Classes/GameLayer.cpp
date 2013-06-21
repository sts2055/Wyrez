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
    
    m_pDraw = CCDrawNode::create();
    addChild(m_pDraw, 10);
    
    m_pGridOrigins_horizontal = new std::vector<CCPoint*>();
    m_pGridOrigins_vertical = new std::vector<CCPoint*>();
    m_pSquares = new std::vector<Square*>();
    
    
    CCLayer* layer = CCLayer::create();
    layer->setContentSize(CCSizeMake(m_squareSide * kSquareXCount, m_squareSide * kSquareYCount));
    auto scrollView = CCScrollView::create(m_visibleSize, layer);
    scrollView->setBounceable(false);
    scrollView->setFMinScale(0.1f);
    scrollView->setFMaxScale(1.0f);
    scrollView->setDelegate(this);
    
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
    
    
    CCSprite *background = createQuickSprite(CCSizeMake(m_visibleSize.width, m_visibleSize.height), kCOLOR_GRAY_07);
    background->setAnchorPoint(CCPointZero);
    background->setPosition(CCPointZero);
    this->addChild(background);
    
    this->addChild(scrollView);
    this->addChild(Hud::createWithParent(*this));
    
    // Setup the grid
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

void GameLayer::scrollViewDidScroll(CCScrollView* view)
{
    m_pDraw->clear();
    
    
    
    // the view's offset is negative
    CCPoint offset = view->getContentOffset();
    
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
    
    
    int k = 0;
    for (int i = vIndex; i < vIndex_max; i++)
    {
        for (int j = i * kSquareXCount + hIndex; j < i * kSquareXCount + hIndex + hLinesPerScreen; j++)
        {
            Square *square = m_pSquares->at(j);
            
            CCPoint points[] = {
                CCPoint((square->origin.x * m_scale)+ offset.x,                  (square->origin.y * m_scale) + offset.y),
                CCPoint((square->origin.x * m_scale) + offset.x + m_squareSide,   (square->origin.y * m_scale) + offset.y),
                CCPoint((square->origin.x * m_scale) + offset.x + m_squareSide,   (square->origin.y * m_scale) + offset.y + m_squareSide),
                CCPoint((square->origin.x * m_scale) + offset.x,                  (square->origin.y * m_scale) + offset.y + m_squareSide)
            };
            m_pDraw->drawPolygon(points, sizeof(points)/sizeof(points[0]), ccc4f(0.5,0,0,1), 0, ccc4f(0,0,1,1));
            k++;
        }
    }
    std::cout << "k:" << k << "\n";
    
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
        m_pDraw->drawSegment(fromPoint, toPoint, 0.5f, ccc4FFromccc3B(kCOLOR_GRAY_06));
    }
    
    for (auto cIter = m_pGridOrigins_horizontal->cbegin() + hIndex;
         cIter != m_pGridOrigins_horizontal->cbegin() + hIndex_max; ++cIter)
    {
        CCPoint fromPoint = CCPointMake(0, (*cIter)->y + offset.y);
        CCPoint toPoint = CCPointMake(m_visibleSize.width, fromPoint.y);
        m_pDraw->drawSegment(fromPoint, toPoint, 0.5f, ccc4FFromccc3B(kCOLOR_GRAY_06));
    }
}

void GameLayer::scrollViewDidZoom(CCScrollView* view)
{
    CCNode* container = view->getContainer();
    m_squareSide = floor(kSquareSide * container->getScale());
    m_scale = container->getScale();
    
    int i = 0;
    for (auto cIter = m_pGridOrigins_horizontal->begin();
         cIter != m_pGridOrigins_horizontal->end(); ++cIter)
    {
        (*cIter)->y = i * m_squareSide;
        i++;
    }
    
    i = 0;
    for (auto cIter = m_pGridOrigins_vertical->begin();
         cIter != m_pGridOrigins_vertical->end(); ++cIter)
    {
        (*cIter)->x = i * m_squareSide;
        i++;
    }
}
