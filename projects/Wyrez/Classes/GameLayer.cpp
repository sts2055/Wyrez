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
GameLayer::GameLayer(const GameScene& rParent, WyrezMap& rWyrezMap)
: m_rParentScene(rParent)
, m_rWyrezMap(rWyrezMap)
, m_wasScrolling(false)
, m_wasZooming(false)
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

void GameLayer::registerWithTouchDispatcher()
{
    CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, GameLayer::getTouchPriority(), false);
}

bool GameLayer::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
    return true;
}

void GameLayer::ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent)
{
    m_wasScrolling = true;
}

void GameLayer::ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent)
{
    // Do not toggle fill when scrolling
    if (m_wasScrolling) {
        m_wasScrolling = false;
        return;
    }
    // Do not toggle fill when zooming
    if (m_wasZooming) {
        m_wasZooming = false;
        return;
    }
    
    CCPoint touchLocation = this->convertTouchToNodeSpace(pTouch);
    //std::cout << "ccTouchEnded - location x:" << touchLocation.x << " y:" << touchLocation.y << "\n";
    m_rWyrezMap.toggleFillForTouchLocation(touchLocation);
}

void GameLayer::ccTouchCancelled(CCTouch *pTouch, CCEvent *pEvent)
{
    std::cout << "ccTouchCancelled:\n";
}

GameLayer* GameLayer::createWithSceneAndMap(const GameScene& rParentScene,
                                            WyrezMap& rWyrezMap)
{
    GameLayer* pRet = new GameLayer(rParentScene, rWyrezMap);
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
GameDrawNode::GameDrawNode(const GameScene& rParentScene, WyrezMap& rWyrezMap)
: m_rParentScene(rParentScene)
, m_rWyrezMap(rWyrezMap)
, m_squareSide(0)
, m_scale(0)
, m_squareFillColor(kCOLOR_BLACK)
, m_squareChargedColor(kCOLOR_BLACK)
, m_squareDischargingColor(kCOLOR_BLACK)
{
    
}

GameDrawNode::~GameDrawNode()
{
    CCLOG("cocos2d: deallocing GameDrawNode %p", this);
}

bool GameDrawNode::init()
{
    if ( !CCDrawNode::init() )
    {
        return false;
    }
    
    m_squareSide = kSquareSide;
    m_scale = kDefaultScale;
    
    m_squareFillColor = kCOLOR_ORANGE;
    m_squareChargedColor = kCOLOR_BLUE;
    m_squareDischargingColor = kCOLOR_WHITE;
    
    return true;
}

GameDrawNode* GameDrawNode::createWithSceneAndMap(const GameScene& rParentScene, WyrezMap& rWyrezMap)
{
    GameDrawNode* pRet = new GameDrawNode(rParentScene, rWyrezMap);
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

void GameDrawNode::redraw(CCScrollView* pView)
{
    this->clear();
    
    // the view's offset is negative
    CCPoint offset = pView->getContentOffset();
    
    // - calculate the horizontal and vertical start indexes for the vector to only iterate over the points that are on screen
    // - MIN because h or vIndex can be above the max index of the vector when zooming in/out at the very left/top edge
    // - Right now it shows either no h or v grid when at the very edge and zooming in/out as it displays areas that are out of bounds
    // - This could be prevented by for example adding more gridOrigins to the vectors
    int vIndex = MIN(floor(-offset.x/m_squareSide), m_rWyrezMap.getSquaresCountVertical());
    int hIndex = MIN(floor(-offset.y/m_squareSide), m_rWyrezMap.getSquaresCountHorizontal());
    
    int vLinesPerScreen = m_rParentScene.getVisibleSize().width/m_squareSide + 2;
    int hLinesPerScreen = m_rParentScene.getVisibleSize().height/m_squareSide + 2;
    
    // - MIN because h or vIndex_max can be above the max index of the vector when scrolling to the very left/top edge
    int vIndex_max = MIN(vIndex + vLinesPerScreen, m_rWyrezMap.getSquaresCountVertical()) ;
    int hIndex_max = MIN(hIndex + hLinesPerScreen, m_rWyrezMap.getSquaresCountHorizontal());
    
    
    //int k = 0;
    for (int i = vIndex; i < vIndex_max; i++)
    {
        for (int j = i * m_rWyrezMap.getSquaresCountVertical() + hIndex;
             j < i * m_rWyrezMap.getSquaresCountVertical() + hIndex + hLinesPerScreen; j++)
        {
            if (j >= m_rWyrezMap.getSquaresCountTotal()) {
                break;
            }
            
            Square *square = m_rWyrezMap.m_pSquares_all->at(j);
            
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
                    this->drawPolygon(points, sizeof(points)/sizeof(points[0]), kCOLOR_ORANGE, 0, kCOLOR_BLACK);
                    break;
                case kSquareChargeStateCharged:
                    this->drawPolygon(points, sizeof(points)/sizeof(points[0]), kCOLOR_BLUE, 0, kCOLOR_BLACK);
                    break;
                case kSquareChargeStateDischarging:
                    this->drawPolygon(points, sizeof(points)/sizeof(points[0]), kCOLOR_WHITE, 0, kCOLOR_BLACK);
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
    
    for (auto cIter = m_rWyrezMap.m_pGridOrigins_vertical->cbegin() + vIndex;
         cIter != m_rWyrezMap.m_pGridOrigins_vertical->cbegin() + vIndex_max; ++cIter)
    {
        CCPoint fromPoint = CCPointMake((*cIter)->x + offset.x, 0);
        CCPoint toPoint = CCPointMake(fromPoint.x, m_rParentScene.getVisibleSize().height);
        this->drawSegment(fromPoint, toPoint, 0.5f, kCOLOR_GRAY_06);
    }
    
    for (auto cIter = m_rWyrezMap.m_pGridOrigins_horizontal->cbegin() + hIndex;
         cIter != m_rWyrezMap.m_pGridOrigins_horizontal->cbegin() + hIndex_max; ++cIter)
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
    for (auto cIter = m_rWyrezMap.m_pGridOrigins_vertical->begin();
         cIter != m_rWyrezMap.m_pGridOrigins_vertical->end(); ++cIter)
    {
        (*cIter)->x = i * m_squareSide;
        i++;
    }
    
    i = 0;
    for (auto cIter = m_rWyrezMap.m_pGridOrigins_horizontal->begin();
         cIter != m_rWyrezMap.m_pGridOrigins_horizontal->end(); ++cIter)
    {
        (*cIter)->y = i * m_squareSide;
        i++;
    }
    
    int k = 0;
    for (i = 0; i < m_rWyrezMap.m_pGridOrigins_vertical->size(); i++)
    {
        for (int j = 0; j < m_rWyrezMap.m_pGridOrigins_horizontal->size(); j++)
        {
            Square *square = m_rWyrezMap.m_pSquares_all->at(k);
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
    
    delete m_pWyrezMap;
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
    
    m_pWyrezMap = new WyrezMap();
    m_pWyrezMap->init();
    
    m_pDraw = GameDrawNode::createWithSceneAndMap(*this, *m_pWyrezMap);
    
    m_pGameLayer = GameLayer::createWithSceneAndMap(*this, *m_pWyrezMap);
    m_pGameLayer->setContentSize(m_pWyrezMap->getContentSize());
    
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
    
    
    this->schedule(schedule_selector(GameScene::gameLogic), 0.05);

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
    this->handleCharges();
    m_pDraw->redraw(m_pScrollView);
}

void GameScene::handleCharges()
{
    std::vector<Square*> squaresToCharge = std::vector<Square*>();
    std::vector<Square*> squaresToDischarge = std::vector<Square*>();
    std::vector<Square*> squaresToNoCharge = std::vector<Square*>();
    for (auto pSquareIter : *m_pWyrezMap->m_pSquares_filled)
    {
        switch (pSquareIter.second->m_chargeState) {
            case kSquareChargeStateNoCharge:
            {
                int electronHeads = 0;
                
                for (auto neighborIter : *pSquareIter.second->m_surroundingSquares)
                {
                    if (neighborIter->m_fillState == kSquareFillStateFilled
                        && neighborIter->m_chargeState == kSquareChargeStateCharged) {
                        electronHeads++;
                    }
                }
                if (electronHeads == 1 || electronHeads == 2) {
                    squaresToCharge.push_back(pSquareIter.second);
                }
                
                break;
            }
  
            case kSquareChargeStateDischarging:
                squaresToNoCharge.push_back(pSquareIter.second);
                break;
            case kSquareChargeStateCharged:
                squaresToDischarge.push_back(pSquareIter.second);
                /*bool shouldDischarge = true;
                for (auto neighborIter : *pSquareIter.second->m_surroundingSquares)
                {
                    if (neighborIter->m_fillState == kSquareFillStateFilled) {
                        if (neighborIter->m_chargeState == kSquareChargeStateNoCharge) {
                            //neighborIter->m_chargeState = kSquareChargeStateCharged;
                            squaresToCharge.push_back(neighborIter);
                        }
                        else {
                            squaresToNoCharge.push_back(neighborIter);
                            squaresToNoCharge.push_back(pSquareIter.second);
                            shouldDischarge = false;
                            //neighborIter->m_chargeState = kSquareChargeStateNoCharge;
                            //pSquareIter.second->m_chargeState = kSquareChargeStateNoCharge;
                        }
                        
                    }
                }
                
                if (shouldDischarge) {
                    squaresToDischarge.push_back(pSquareIter.second);
                }*/
                break;
        }
    }
    
    for (auto pIter : squaresToCharge)
    {
        pIter->m_chargeState = kSquareChargeStateCharged;
    }
    
    for (auto pIter : squaresToDischarge)
    {
        pIter->m_chargeState = kSquareChargeStateDischarging;
    }
    
    for (auto pIter : squaresToNoCharge)
    {
        pIter->m_chargeState = kSquareChargeStateNoCharge;
    }
}

void GameScene::scrollViewDidScroll(CCScrollView* view)
{
    m_pDraw->redraw(view);
}

void GameScene::scrollViewDidZoom(CCScrollView* view)
{
    m_pDraw->rearrange(view);
    m_pGameLayer->setWasZooming(true);
}















