//
//  GameLayer.cpp
//  Wyrez
//
//  Created by stefan on 6/20/13.
//
//

#include "GameLayer.h"

/*****************************************************************************************
 * implementation of GameScrollView
 *****************************************************************************************/
#define MOVE_INCH            7.0f/160.0f
static float convertDistanceFromPointToInch(float pointDis)
{
    float factor = ( CCEGLView::sharedOpenGLView()->getScaleX() + CCEGLView::sharedOpenGLView()->getScaleY() ) / 2;
    return pointDis * factor / CCDevice::getDPI();
}

GameScrollView::GameScrollView()
: m_shouldScroll(true)
{
    
}

GameScrollView::~GameScrollView()
{
    CCLOG("cocos2d: deallocing ~GameScrollView %p", this);
}

GameScrollView* GameScrollView::create(CCSize size, CCNode* container/* = NULL*/)
{
    GameScrollView* pRet = new GameScrollView();
    if (pRet && pRet->initWithViewSize(size, container))
    {
        pRet->autorelease();
    }
    else
    {
        CC_SAFE_DELETE(pRet);
    }
    return pRet;
}

void GameScrollView::ccTouchMoved(CCTouch* touch, CCEvent* event)
{
    if (!m_shouldScroll) {
        return;
    }
    
    if (!this->isVisible())
    {
        return;
    }
    
    if (m_pTouches->containsObject(touch))
    {
        if (m_pTouches->count() == 1 && m_bDragging)
        { // scrolling
            CCPoint moveDistance, newPoint, maxInset, minInset;
            CCRect  frame;
            float newX, newY;
            
            frame = getViewRect();
            
            newPoint     = this->convertTouchToNodeSpace((CCTouch*)m_pTouches->objectAtIndex(0));
            moveDistance = ccpSub(newPoint, m_tTouchPoint);
            
            float dis = 0.0f;
            if (m_eDirection == kCCScrollViewDirectionVertical)
            {
                dis = moveDistance.y;
            }
            else if (m_eDirection == kCCScrollViewDirectionHorizontal)
            {
                dis = moveDistance.x;
            }
            else
            {
                dis = sqrtf(moveDistance.x*moveDistance.x + moveDistance.y*moveDistance.y);
            }
            
            if (!m_bTouchMoved && fabs(convertDistanceFromPointToInch(dis)) < MOVE_INCH )
            {
                //CCLOG("Invalid movement, distance = [%f, %f], disInch = %f", moveDistance.x, moveDistance.y);
                return;
            }
            
            if (!m_bTouchMoved)
            {
                moveDistance = CCPointZero;
            }
            
            m_tTouchPoint = newPoint;
            m_bTouchMoved = true;
            
            if (frame.containsPoint(this->convertToWorldSpace(newPoint)))
            {
                switch (m_eDirection)
                {
                    case kCCScrollViewDirectionVertical:
                        moveDistance = ccp(0.0f, moveDistance.y);
                        break;
                    case kCCScrollViewDirectionHorizontal:
                        moveDistance = ccp(moveDistance.x, 0.0f);
                        break;
                    default:
                        break;
                }
                
                maxInset = m_fMaxInset;
                minInset = m_fMinInset;
                
                newX     = m_pContainer->getPosition().x + moveDistance.x;
                newY     = m_pContainer->getPosition().y + moveDistance.y;
                
                m_tScrollDistance = moveDistance;
                this->setContentOffset(ccp(newX, newY));
            }
        }
        else if (m_pTouches->count() == 2 && !m_bDragging)
        {
            const float len = ccpDistance(m_pContainer->convertTouchToNodeSpace((CCTouch*)m_pTouches->objectAtIndex(0)),
                                          m_pContainer->convertTouchToNodeSpace((CCTouch*)m_pTouches->objectAtIndex(1)));
            this->setZoomScale( floorf((this->getZoomScale()*len/m_fTouchLength) * 10 + 0.5) / 10 );
            m_bZooming = true;
        }
    }
}


/*****************************************************************************************
 * implementation of GameHud
 *****************************************************************************************/
GameHud::GameHud(GameScene& rParentScene, WyrezMap& rWyrezMap)
: m_rParentScene(rParentScene)
, m_rWyrezMap(rWyrezMap)
, m_active_menu(nullptr)
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
    
    this->loadMenu_displayMode();
    
    return true;
}

GameHud* GameHud::createWithParentScene(GameScene& rParentScene, WyrezMap& rWyrezMap)
{
    GameHud* pRet = new GameHud(rParentScene, rWyrezMap);
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

CCSprite* GameHud::createCustomSprite(std::string name, ccColor3B color)
{
    CCSprite* sprite = new CCSprite();
    sprite->initWithFile(name.c_str());
    sprite->setColor(color);
    sprite->autorelease();
    return sprite;
}

CCMenuItemSprite* GameHud::createMenuItemSpriteWithIcon(std::string icon, SEL_MenuHandler selector)
{
    CCSprite* pNormalSprite = this->createCustomSprite(std::string("build_menu_button_frame.png"), m_rWyrezMap.getSquareChargedColor());
    CCSprite* pNormalSprite_icon = this->createCustomSprite(icon, m_rWyrezMap.getSquareDischargingColor());
    pNormalSprite_icon->setPosition(ccp(pNormalSprite->getContentSize().width/2, pNormalSprite->getContentSize().height/2));
    pNormalSprite->addChild(pNormalSprite_icon);
    
    CCSprite* pSelectedSprite = this->createCustomSprite(std::string("build_menu_button_frame.png"), m_rWyrezMap.getSquareChargedColor());
    
    
    CCMenuItemSprite *pRet = new CCMenuItemSprite();
    pRet->initWithNormalSprite(pNormalSprite, pSelectedSprite, nullptr, this, selector);
    pRet->autorelease();
    
    return pRet;
}

void GameHud::loadMenu_displayMode()
{
    if (m_active_menu != nullptr){
        m_active_menu->removeFromParent();
    }

    CCMenuItemSprite* button_build = this->createMenuItemSpriteWithIcon(std::string("build_menu_icon_wrenchscrewdriver.png"),
                                                                        menu_selector(GameHud::loadMenu_buildMode));
    int margin = m_rParentScene.m_visibleSize.width * 0.01;
    button_build->setPosition(ccp(m_rParentScene.m_visibleSize.width - button_build->getNormalImage()->getContentSize().width/2 - margin,
                                  m_rParentScene.m_visibleSize.height - button_build->getNormalImage()->getContentSize().height/2 - margin));
    
    CCArray* pMenuItems = CCArray::createWithCapacity(1);
    pMenuItems->addObject(button_build);
    
    CCMenu* pMenu = CCMenu::createWithArray(pMenuItems);
    pMenu->setAnchorPoint(CCPointZero);
    pMenu->setPosition(CCPointZero);
    this->addChild(pMenu);
    m_active_menu = pMenu;
}

void GameHud::loadMenu_buildMode()
{
    if (m_active_menu != nullptr){
        m_active_menu->removeFromParent();
    }
    
    CCMenuItemSprite* button_build = this->createMenuItemSpriteWithIcon(std::string("build_menu_icon_wrenchscrewdriver.png"),
                                                                        menu_selector(GameHud::loadMenu_displayMode));
    int margin = m_rParentScene.m_visibleSize.width * 0.01;
    button_build->setPosition(ccp(m_rParentScene.m_visibleSize.width - button_build->getNormalImage()->getContentSize().width/2 - margin,
                                  m_rParentScene.m_visibleSize.height - button_build->getNormalImage()->getContentSize().height/2 - margin));
    
    CCMenuItemSprite* button_info = this->createMenuItemSpriteWithIcon(std::string("build_menu_icon_info.png"),
                                                                        menu_selector(GameHud::loadMenu_displayMode));
    button_info->setPosition(ccp(button_build->getPosition().x - button_info->getNormalImage()->getContentSize().width - margin,
                                  button_build->getPosition().y));
    
    
    
    CCSprite* pNormalSprite = this->createCustomSprite(std::string("build_menu_button_frame.png"), m_rWyrezMap.getSquareChargedColor());
    CCSprite* pNormalSprite_icon = this->createCustomSprite(std::string("build_menu_icon_brush.png"), m_rWyrezMap.getSquareDischargingColor());
    pNormalSprite_icon->setPosition(ccp(pNormalSprite->getContentSize().width/2, pNormalSprite->getContentSize().height/2));
    pNormalSprite->addChild(pNormalSprite_icon);
    
    CCSprite* pSelectedSprite = this->createCustomSprite(std::string("build_menu_button_frame.png"), m_rWyrezMap.getSquareChargedColor());
    
    
    BrushMenuItemSprite *brush = new BrushMenuItemSprite();
    brush->initWithNormalSprite(pNormalSprite, pSelectedSprite, nullptr, this, menu_selector(GameHud::doNothing));
    brush->setDelegate(this);
    brush->autorelease();
    brush->setPosition(ccp(0 + brush->getNormalImage()->getContentSize().width/2 + margin,
                                     m_rParentScene.m_visibleSize.height * 0.5 + brush->getNormalImage()->getContentSize().height/2));
    
    
    CCMenuItemSprite* button_freedraw = this->createMenuItemSpriteWithIcon(std::string("build_menu_icon_brush.png"),
                                                                           menu_selector(GameHud::doNothing));
    button_freedraw->setPosition(ccp(0 + button_freedraw->getNormalImage()->getContentSize().width/2 + margin,
                                  m_rParentScene.m_visibleSize.height * 0.5 + button_freedraw->getNormalImage()->getContentSize().height/2));
    
    CCArray* pMenuItems = CCArray::createWithCapacity(1);
    pMenuItems->addObject(button_build);
    pMenuItems->addObject(button_info);
    pMenuItems->addObject(brush);
    
    CCMenu* pMenu = CCMenu::createWithArray(pMenuItems);
    pMenu->setAnchorPoint(CCPointZero);
    pMenu->setPosition(CCPointZero);
    this->addChild(pMenu);
    m_active_menu = pMenu;
}

void GameHud::brushMenuItemSpriteIsSelected()
{
    m_rParentScene.disableScrolling();
}

void GameHud::brushMenuItemSpriteIsUnselected() // TODO might not need this one
{
    m_rParentScene.enableScrolling();
}






/*****************************************************************************************
 * implementation of GameLayer
 *****************************************************************************************/
GameLayer::GameLayer(GameScene& rParent, WyrezMap& rWyrezMap)
: m_rParentScene(rParent)
, m_rWyrezMap(rWyrezMap)
, m_wasScrolling(false)
, m_wasZooming(false)
, m_brushModeActive(false)
, m_brushModeCurrentSquare(nullptr)
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
    
    if (m_brushModeActive) {
        CCPoint touchLocation = this->convertTouchToNodeSpace(pTouch);
        CCPoint squareLocation = ccp(floor(touchLocation.x/kSquareSide), floor(touchLocation.y/kSquareSide));
        int index = squareLocation.x * m_rWyrezMap.getSquaresCountHorizontal() + squareLocation.y;
        Square* pSquare = m_rWyrezMap.m_pSquares_all->at(index);
        
        // If the touched square is new, we toggle its fillstate and draw it
        // This is to prevent multiple toggles and draws on the same square as the touch moves
        if (m_brushModeCurrentSquare == nullptr || m_brushModeCurrentSquare != pSquare) {
            m_brushModeCurrentSquare = pSquare;
            m_rWyrezMap.toggleFillForTouchLocation(touchLocation);
            m_rParentScene.redrawDrawNode();
        }
    }
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
    m_rWyrezMap.toggleFillForTouchLocation(touchLocation);
}

void GameLayer::ccTouchCancelled(CCTouch *pTouch, CCEvent *pEvent)
{
    std::cout << "ccTouchCancelled:\n";
}

GameLayer* GameLayer::createWithSceneAndMap(GameScene& rParentScene,
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
, m_gridLinesColor({0.0,0.0,0.0,0.0})
, m_squareFillColor({0.0,0.0,0.0,0.0})
, m_squareChargedColor({0.0,0.0,0.0,0.0})
, m_squareDischargingColor({0.0,0.0,0.0,0.0})
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
    
    m_gridLinesColor = ccc4FFromccc3B(m_rWyrezMap.getGridLinesColor());
    m_squareFillColor = ccc4FFromccc3B(m_rWyrezMap.getSquareFillColor());
    m_squareChargedColor = ccc4FFromccc3B(m_rWyrezMap.getSquareChargedColor());
    m_squareDischargingColor = ccc4FFromccc3B(m_rWyrezMap.getSquareDischargingColor());
    
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
    
    int vLinesPerScreen = m_rParentScene.m_visibleSize.width/m_squareSide + 2;
    int hLinesPerScreen = m_rParentScene.m_visibleSize.height/m_squareSide + 2;
    
    // - MIN because h or vIndex_max can be above the max index of the vector when scrolling to the very left/top edge
    int vIndex_max = MIN(vIndex + vLinesPerScreen, m_rWyrezMap.getSquaresCountVertical()) ;
    int hIndex_max = MIN(hIndex + hLinesPerScreen, m_rWyrezMap.getSquaresCountHorizontal());
    
    
    //int k = 0;
    for (int i = vIndex; i < vIndex_max; i++)
    {
        for (int j = i * m_rWyrezMap.getSquaresCountVertical() + hIndex;
             j < i * m_rWyrezMap.getSquaresCountVertical() + hIndex + hLinesPerScreen; j++)
        {
            if (j >= m_rWyrezMap.m_pSquares_all->size() - 1) {
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
                    this->drawPolygon(points, sizeof(points)/sizeof(points[0]), m_squareFillColor, 0, kCOLOR_BLACK);
                    break;
                case kSquareChargeStateCharged:
                    this->drawPolygon(points, sizeof(points)/sizeof(points[0]), m_squareChargedColor, 0, kCOLOR_BLACK);
                    break;
                case kSquareChargeStateDischarging:
                    this->drawPolygon(points, sizeof(points)/sizeof(points[0]), m_squareDischargingColor, 0, kCOLOR_BLACK);
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
        CCPoint toPoint = CCPointMake(fromPoint.x, m_rParentScene.m_visibleSize.height);
        this->drawSegment(fromPoint, toPoint, 0.5f, m_gridLinesColor);
    }
    
    for (auto cIter = m_rWyrezMap.m_pGridOrigins_horizontal->cbegin() + hIndex;
         cIter != m_rWyrezMap.m_pGridOrigins_horizontal->cbegin() + hIndex_max; ++cIter)
    {
        CCPoint fromPoint = CCPointMake(0, (*cIter)->y + offset.y);
        CCPoint toPoint = CCPointMake(m_rParentScene.m_visibleSize.width, fromPoint.y);
        this->drawSegment(fromPoint, toPoint, 0.5f, m_gridLinesColor);
    }
}

void GameDrawNode::rearrange(CCScrollView* pView)
{
    CCNode* container = pView->getContainer();
    m_squareSide = kSquareSide * container->getScale();
    
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
    
    m_pWyrezMap = new WyrezMap();
    m_pWyrezMap->init();
    
    CCSprite *background = createQuickSprite(CCSizeMake(m_visibleSize.width, m_visibleSize.height), m_pWyrezMap->getBackgroundColor());
    background->setAnchorPoint(CCPointZero);
    background->setPosition(CCPointZero);
    
    m_pDraw = GameDrawNode::createWithSceneAndMap(*this, *m_pWyrezMap);
    
    m_pGameLayer = GameLayer::createWithSceneAndMap(*this, *m_pWyrezMap);
    m_pGameLayer->setContentSize(m_pWyrezMap->getContentSize());
    
    m_pScrollView = GameScrollView::create(m_visibleSize, m_pGameLayer);
    m_pScrollView->setBounceable(false);
    // the scrollview should not zoom out to show space beyond its bounds
    // (it would cause an out of bounds exception in one of the vectors)
    // - it may only zoom in increments/decrements of 0.1
    // - the zoom out scale is limited to 0.2 or higher (rounded up) if one axis is shorter than what would be displayed at 0.2
    float xMinScale = ceilf(m_visibleSize.width / m_pWyrezMap->getContentSize().width * 10) / 10;
    xMinScale = MAX(xMinScale, 0.2);
    float yMinScale = ceilf(m_visibleSize.height / m_pWyrezMap->getContentSize().height * 10) / 10;
    yMinScale = MAX(yMinScale, 0.2);
    m_pScrollView->setFMinScale(yMinScale > xMinScale ? yMinScale : xMinScale);
    m_pScrollView->setFMaxScale(1.0f);
    m_pScrollView->setDelegate(this);
    
    m_pGameHud = GameHud::createWithParentScene(*this, *m_pWyrezMap);
    
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

void GameScene::disableScrolling()
{
    m_pScrollView->setShouldScroll(false);
    m_pGameLayer->setBrushModeActive(true);
}

void GameScene::enableScrolling()
{
    m_pScrollView->setShouldScroll(true);
    m_pGameLayer->setBrushModeActive(false);
}

void GameScene::redrawDrawNode()
{
    m_pDraw->redraw(m_pScrollView);
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















