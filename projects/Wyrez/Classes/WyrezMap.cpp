//
//  WyrezMap.cpp
//  Wyrez
//
//  Created by stefan on 6/24/13.
//
//

#include "WyrezMap.h"
#include <fstream>
#include "DebuggingHelpers.h"

static const int kDefaultSquaresVertical = 100;
static const int kDefaultSquaresHorizontal = 100;

static const ccColor3B kDEFAULT_COLOR_BACKGROUND    = {63,  63, 63}; // dark gray
static const ccColor3B kDEFAULT_COLOR_GRIDLINES     = {99,  99, 99}; // lighter gray
static const ccColor3B kDEFAULT_COLOR_FILL          = {255, 147,36}; // orange
static const ccColor3B kDEFAULT_COLOR_CHARGED       = {13,  153,252}; // light blue
static const ccColor3B kDEFAULT_COLOR_DISCHARGING   = {255, 255,255}; // white


WyrezMap::WyrezMap()
: m_squareSide(0)
, m_squaresCount_vertical(0)
, m_squaresCount_horizontal(0)
, m_squaresCount_total(0)
, m_backgroundColor({0,0,0})
, m_gridLinesColor({0,0,0})
, m_squareFillColor({0,0,0})
, m_squareChargedColor({0,0,0})
, m_squareDischargingColor({0,0,0})
, m_contentSize(CCSizeZero)
, m_pGridOrigins_vertical(nullptr)
, m_pGridOrigins_horizontal(nullptr)
, m_pSquares_all(nullptr)
, m_pSquares_filled(nullptr)
, m_mapName("")
, m_writeToFileRootDict(nullptr)
{
    
}

WyrezMap::~WyrezMap()
{
    CCLOG("cocos2d: deallocing WyrezMap %p", this);
    
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
    
    for (auto square : *m_pSquares_all)
    {
        delete square;
    }
    delete m_pSquares_all;
    delete m_pSquares_filled;
    
    CC_SAFE_RELEASE(m_writeToFileRootDict);
}

bool WyrezMap::init()
{
    m_squareSide = kDefaultSquareSide;
    m_squaresCount_vertical = kDefaultSquaresVertical;
    m_squaresCount_horizontal = kDefaultSquaresHorizontal;
    m_squaresCount_total = m_squaresCount_vertical * m_squaresCount_horizontal;
    
    m_backgroundColor           = kDEFAULT_COLOR_BACKGROUND;
    m_gridLinesColor            = kDEFAULT_COLOR_GRIDLINES;
    m_squareFillColor           = kDEFAULT_COLOR_FILL;
    m_squareChargedColor        = kDEFAULT_COLOR_CHARGED;
    m_squareDischargingColor    = kDEFAULT_COLOR_DISCHARGING;
    
    m_contentSize = CCSizeMake(m_squaresCount_vertical * kDefaultSquareSide, m_squaresCount_horizontal * kDefaultSquareSide);
    
    m_pGridOrigins_vertical = new std::vector<CCPoint*>();
    m_pGridOrigins_horizontal = new std::vector<CCPoint*>();
    m_pSquares_all = new std::vector<Square*>();
    m_pSquares_filled = new std::map<int, Square*>();
    
    int verticalLines = m_squaresCount_vertical;
    int horizontalLines = m_squaresCount_horizontal;
    
    for (int i = 0; i < verticalLines; i++)
    {
        m_pGridOrigins_vertical->push_back(new CCPoint(i * kDefaultSquareSide, 0));
    }
    
    for (int i = 0; i < horizontalLines; i++)
    {
        m_pGridOrigins_horizontal->push_back(new CCPoint(0, i * kDefaultSquareSide));
    }
    
    // This makes the axis for vertical lines (x) the first axis when determining the index of a square inside the array
    // e.g. assuming the map is 100x100 squares (0-based), a x-value of 3 means that the square is within an index range of 300 - 400
    int index = 0;
    for (int i = 0; i < verticalLines; i++)
    {
        for (int j = 0; j < horizontalLines; j++)
        {
            m_pSquares_all->push_back(new Square(index,
                                                 i * kDefaultSquareSide,
                                                 j * kDefaultSquareSide,
                                                 kDefaultSquareSide,
                                                 kDefaultSquareSide));
            index++;
        }
    }
    this->setupSurroundingSquares();
    
    std::cout << "sizeof(Square): " << sizeof(Square) << "\n";
    std::cout << "sizeof(Square*): " << sizeof(Square*) << "\n";
    std::cout << "sizeof(CCPoint): " << sizeof(CCPoint) << "\n";
    std::cout << "sizeof(CCSize): " << sizeof(CCSize) << "\n";
    std::cout << "sizeof(float): " << sizeof(float) << "\n";
    std::cout << "sizeof(kSquareFillStateEmpty): " << sizeof(kSquareFillStateEmpty) << "\n";
    
    // Setup the first squares TEMP
    Square* square0 = m_pSquares_all->at(1);
    square0->m_fillState = kSquareFillStateFilled;
    square0->m_chargeState = kSquareChargeStateDischarging;
    m_pSquares_filled->insert(std::make_pair(square0->m_index, square0));
    
    Square* square1 = m_pSquares_all->at(2);
    square1->m_fillState = kSquareFillStateFilled;
    square1->m_chargeState = kSquareChargeStateCharged;
    m_pSquares_filled->insert(std::make_pair(square1->m_index, square1));
    
    Square* square2 = m_pSquares_all->at(3 + m_squaresCount_horizontal);
    square2->m_fillState = kSquareFillStateFilled;
    m_pSquares_filled->insert(std::make_pair(square2->m_index, square2));
    
    Square* square3 = m_pSquares_all->at(2 + m_squaresCount_horizontal * 2);
    square3->m_fillState = kSquareFillStateFilled;
    m_pSquares_filled->insert(std::make_pair(square3->m_index, square3));
    
    Square* square4 = m_pSquares_all->at(1 + m_squaresCount_horizontal * 2);
    square4->m_fillState = kSquareFillStateFilled;
    m_pSquares_filled->insert(std::make_pair(square4->m_index, square4));
    
    Square* square5 = m_pSquares_all->at(0 + m_squaresCount_horizontal);
    square5->m_fillState = kSquareFillStateFilled;
    m_pSquares_filled->insert(std::make_pair(square5->m_index, square5));
    
    return true;
}

void WyrezMap::setupSurroundingSquares()
{
    for (int i = 0; i < m_pSquares_all->size(); i++)
    {
        Square* pSquare = m_pSquares_all->at(i);
        int squarePosX = pSquare->origin.x/m_squareSide;
        int squarePosY = pSquare->origin.y/m_squareSide;
        
        /*
         ooo
         oxo
         ooo
         */
        if (         squarePosX > 0
                 &&  squarePosX < m_squaresCount_vertical-1
                 &&  squarePosY > 0
                 &&  squarePosY < m_squaresCount_horizontal-1)
        {
            pSquare->m_surroundingSquares->push_back(m_pSquares_all->at(    i - m_squaresCount_horizontal - 1   ));
            pSquare->m_surroundingSquares->push_back(m_pSquares_all->at(    i - m_squaresCount_horizontal       ));
            pSquare->m_surroundingSquares->push_back(m_pSquares_all->at(    i - m_squaresCount_horizontal + 1   ));
            pSquare->m_surroundingSquares->push_back(m_pSquares_all->at(    i - 1                               ));
            pSquare->m_surroundingSquares->push_back(m_pSquares_all->at(    i + 1                               ));
            pSquare->m_surroundingSquares->push_back(m_pSquares_all->at(    i + m_squaresCount_horizontal - 1   ));
            pSquare->m_surroundingSquares->push_back(m_pSquares_all->at(    i + m_squaresCount_horizontal       ));
            pSquare->m_surroundingSquares->push_back(m_pSquares_all->at(    i + m_squaresCount_horizontal + 1   ));
        }
        /*
         |oo
         |xo
         |oo
         */
        else if (    squarePosX == 0
                 &&  squarePosX < m_squaresCount_vertical-1
                 &&  squarePosY > 0
                 &&  squarePosY < m_squaresCount_horizontal-1)
        {
            pSquare->m_surroundingSquares->push_back(m_pSquares_all->at(    i - 1                               ));
            pSquare->m_surroundingSquares->push_back(m_pSquares_all->at(    i + 1                               ));
            pSquare->m_surroundingSquares->push_back(m_pSquares_all->at(    i + m_squaresCount_horizontal - 1   ));
            pSquare->m_surroundingSquares->push_back(m_pSquares_all->at(    i + m_squaresCount_horizontal       ));
            pSquare->m_surroundingSquares->push_back(m_pSquares_all->at(    i + m_squaresCount_horizontal + 1   ));
        }
        /*
         oo|
         ox|
         oo|
         */
        else if (    squarePosX > 0
                 &&  squarePosX == m_squaresCount_vertical-1
                 &&  squarePosY > 0
                 &&  squarePosY < m_squaresCount_horizontal-1)
        {
            pSquare->m_surroundingSquares->push_back(m_pSquares_all->at(    i - m_squaresCount_horizontal - 1   ));
            pSquare->m_surroundingSquares->push_back(m_pSquares_all->at(    i - m_squaresCount_horizontal       ));
            pSquare->m_surroundingSquares->push_back(m_pSquares_all->at(    i - m_squaresCount_horizontal + 1   ));
            pSquare->m_surroundingSquares->push_back(m_pSquares_all->at(    i - 1                               ));
            pSquare->m_surroundingSquares->push_back(m_pSquares_all->at(    i + 1                               ));
        }
        /*
         ooo
         oxo
         ---
         */
        else if (    squarePosX > 0
                 &&  squarePosX < m_squaresCount_vertical-1
                 &&  squarePosY == 0
                 &&  squarePosY < m_squaresCount_horizontal-1)
        {
            pSquare->m_surroundingSquares->push_back(m_pSquares_all->at(    i - m_squaresCount_horizontal       ));
            pSquare->m_surroundingSquares->push_back(m_pSquares_all->at(    i - m_squaresCount_horizontal + 1   ));
            pSquare->m_surroundingSquares->push_back(m_pSquares_all->at(    i + 1                               ));
            pSquare->m_surroundingSquares->push_back(m_pSquares_all->at(    i + m_squaresCount_horizontal       ));
            pSquare->m_surroundingSquares->push_back(m_pSquares_all->at(    i + m_squaresCount_horizontal + 1   ));
        }
        /*
         ---
         oxo
         ooo
         */
        else if (    squarePosX > 0
                 &&  squarePosX < m_squaresCount_vertical-1
                 &&  squarePosY > 0
                 &&  squarePosY == m_squaresCount_horizontal-1)
        {
            pSquare->m_surroundingSquares->push_back(m_pSquares_all->at(    i - m_squaresCount_horizontal - 1   ));
            pSquare->m_surroundingSquares->push_back(m_pSquares_all->at(    i - m_squaresCount_horizontal       ));
            pSquare->m_surroundingSquares->push_back(m_pSquares_all->at(    i - 1                               ));
            pSquare->m_surroundingSquares->push_back(m_pSquares_all->at(    i + m_squaresCount_horizontal - 1   ));
            pSquare->m_surroundingSquares->push_back(m_pSquares_all->at(    i + m_squaresCount_horizontal       ));
        }
        /*
         |oo
         |xo
         |--
         */
        else if (    squarePosX == 0
                 &&  squarePosX < m_squaresCount_vertical-1
                 &&  squarePosY == 0
                 &&  squarePosY < m_squaresCount_horizontal-1)
        {
            pSquare->m_surroundingSquares->push_back(m_pSquares_all->at(    i + 1                               ));
            pSquare->m_surroundingSquares->push_back(m_pSquares_all->at(    i + m_squaresCount_horizontal       ));
            pSquare->m_surroundingSquares->push_back(m_pSquares_all->at(    i + m_squaresCount_horizontal + 1   ));
        }
        /*
         oo|
         ox|
         --|
         */
        else if (    squarePosX > 0
                 &&  squarePosX == m_squaresCount_vertical-1
                 &&  squarePosY == 0
                 &&  squarePosY < m_squaresCount_horizontal-1)
        {
            pSquare->m_surroundingSquares->push_back(m_pSquares_all->at(    i - m_squaresCount_horizontal       ));
            pSquare->m_surroundingSquares->push_back(m_pSquares_all->at(    i - m_squaresCount_horizontal + 1   ));
            pSquare->m_surroundingSquares->push_back(m_pSquares_all->at(    i + 1                               ));
        }
        /*
         |--
         |xo
         |oo
         */
        else if (    squarePosX == 0
                 &&  squarePosX < m_squaresCount_vertical-1
                 &&  squarePosY > 0
                 &&  squarePosY == m_squaresCount_horizontal-1)
        {
            pSquare->m_surroundingSquares->push_back(m_pSquares_all->at(    i - 1                               ));
            pSquare->m_surroundingSquares->push_back(m_pSquares_all->at(    i + m_squaresCount_horizontal - 1   ));
            pSquare->m_surroundingSquares->push_back(m_pSquares_all->at(    i + m_squaresCount_horizontal       ));
        }
        /*
         --|
         ox|
         oo|
         */
        else if (    squarePosX > 0
                 &&  squarePosX == m_squaresCount_vertical-1
                 &&  squarePosY > 0
                 &&  squarePosY == m_squaresCount_horizontal-1)
        {
            pSquare->m_surroundingSquares->push_back(m_pSquares_all->at(    i - m_squaresCount_horizontal - 1   ));
            pSquare->m_surroundingSquares->push_back(m_pSquares_all->at(    i - m_squaresCount_horizontal       ));
            pSquare->m_surroundingSquares->push_back(m_pSquares_all->at(    i - 1                               ));
        }
    }
}

void WyrezMap::toggleFillForTouchLocation(CCPoint touchLocation)
{
    CCPoint squareLocation = ccp(floor(touchLocation.x/m_squareSide), floor(touchLocation.y/m_squareSide));
    int index = squareLocation.x * m_squaresCount_horizontal + squareLocation.y;
    
    Square* pSquare = m_pSquares_all->at(index);
    
    if (pSquare->m_fillState == kSquareFillStateEmpty) {
        pSquare->m_fillState = kSquareFillStateFilled;
        m_pSquares_filled->insert(std::make_pair(pSquare->m_index, pSquare));
    }
    else {
        pSquare->m_fillState = kSquareFillStateEmpty;
        pSquare->m_chargeState = kSquareChargeStateNoCharge;
        std::map<int, Square*>::iterator itr = m_pSquares_filled->find(pSquare->m_index);
        m_pSquares_filled->erase(itr);
    }
    
}

void WyrezMap::writeToFile(int step)
{
    auto addToDictFunc = [](CCDictionary* pDict, int val, std::string keyStr) -> void {
        pDict->setObject(CCString::create(std::to_string(val)), keyStr.c_str());
    };
   
    CCDictionary* root = nullptr;
    if(m_writeToFileRootDict == nullptr) {
        m_writeToFileRootDict = CCDictionary::create();
        CC_SAFE_RETAIN(m_writeToFileRootDict);
    }
    root = m_writeToFileRootDict;
    
    switch (step) {
        case 1:
        {
            CCString *mapName = CCString::create(m_mapName.empty() ? "default" : m_mapName);
            root->setObject(mapName, "string element key");
            break;
        }
        case 2:
        {
            CCDictionary *dimensionsDict = CCDictionary::create();
            addToDictFunc(dimensionsDict, m_squareSide,                 "squareside");
            addToDictFunc(dimensionsDict, m_squaresCount_vertical,      "squaresx"); //vertical squares
            addToDictFunc(dimensionsDict, m_squaresCount_horizontal,    "squaresy"); //horizontal squares
            root->setObject(dimensionsDict, "dimensions");
            break;
        }
        case 3:
        {
            CCDictionary *colorsDict = CCDictionary::create();
            {
                CCDictionary *colorDict_background = CCDictionary::create();
                addToDictFunc(colorDict_background, m_backgroundColor.r,    "r");
                addToDictFunc(colorDict_background, m_backgroundColor.g,    "g");
                addToDictFunc(colorDict_background, m_backgroundColor.b,    "b");
                colorsDict->setObject(colorDict_background, "background");
            }
            {
                CCDictionary *colorDict_gridLines = CCDictionary::create();
                addToDictFunc(colorDict_gridLines, m_gridLinesColor.r,    "r");
                addToDictFunc(colorDict_gridLines, m_gridLinesColor.g,    "g");
                addToDictFunc(colorDict_gridLines, m_gridLinesColor.b,    "b");
                colorsDict->setObject(colorDict_gridLines, "gridlines");
            }
            {
                CCDictionary *colorDict_squareFill = CCDictionary::create();
                addToDictFunc(colorDict_squareFill, m_squareFillColor.r,    "r");
                addToDictFunc(colorDict_squareFill, m_squareFillColor.g,    "g");
                addToDictFunc(colorDict_squareFill, m_squareFillColor.b,    "b");
                colorsDict->setObject(colorDict_squareFill, "squarefill");
            }
            {
                CCDictionary *colorDict_squareCharge = CCDictionary::create();
                addToDictFunc(colorDict_squareCharge, m_squareChargedColor.r,    "r");
                addToDictFunc(colorDict_squareCharge, m_squareChargedColor.g,    "g");
                addToDictFunc(colorDict_squareCharge, m_squareChargedColor.b,    "b");
                colorsDict->setObject(colorDict_squareCharge, "squarecharged");
            }
            {
                CCDictionary *colorDict_squareDischar = CCDictionary::create();
                addToDictFunc(colorDict_squareDischar, m_squareDischargingColor.r,    "r");
                addToDictFunc(colorDict_squareDischar, m_squareDischargingColor.g,    "g");
                addToDictFunc(colorDict_squareDischar, m_squareDischargingColor.b,    "b");
                colorsDict->setObject(colorDict_squareDischar, "squaredischarging");
            }
            root->setObject(colorsDict, "colors");
            break;
        }
        case 4:
        {
            CCArray *squaresArray = CCArray::create();
            /*for (auto itr : *m_pSquares_all)
            {
                CCDictionary *squareDict = CCDictionary::create();
                //addToDictFunc(squareDict, itr->m_index,         "i"); // index
                //addToDictFunc(squareDict, itr->origin.x,        "x"); // x
                //addToDictFunc(squareDict, itr->origin.y,        "y"); // y
                addToDictFunc(squareDict, itr->m_fillState,     "f"); // fillstate
                addToDictFunc(squareDict, itr->m_chargeState,   "c"); // chargestate
                squaresArray->addObject(squareDict);
            }*/
            
            for (auto iter = m_pSquares_filled->begin(); iter!=m_pSquares_filled->end(); ++iter)
            {
                CCDictionary *squareDict = CCDictionary::create();
                addToDictFunc(squareDict, iter->second->m_index,         "i"); // index
                addToDictFunc(squareDict, iter->second->origin.x,        "x"); // x
                addToDictFunc(squareDict, iter->second->origin.y,        "y"); // y
                //addToDictFunc(squareDict, iter->second->m_fillState,     "f"); // fillstate
                addToDictFunc(squareDict, iter->second->m_chargeState,   "c"); // chargestate
                squaresArray->addObject(squareDict);
            }
            
            
            root->setObject(squaresArray, "squares");
            break;
        }
        case 5:
        {
            std::string writablePath = CCFileUtils::sharedFileUtils()->getWritablePath();
            std::string fullPath = writablePath + "default.plist";
            
            if(root->writeToFile(fullPath.c_str()))
                CCLog("see the plist file at %s", fullPath.c_str());
            else
                CCLog("write plist file failed");
            
            CC_SAFE_RELEASE(m_writeToFileRootDict);
            m_writeToFileRootDict = nullptr;
            printFileAtLocation(fullPath);
            break;
        }
        default:
        {
            CCLOGERROR("ERROR: step %i not handled", step);
            break;
        }
    }
}

void WyrezMap::test(int step)
{
    std::string writablePath = CCFileUtils::sharedFileUtils()->getWritablePath();
    std::string fullPath = writablePath + "default.json";
    std::ofstream file (fullPath);
    
    if (file.is_open()) {
        file << "{\n";
        file << "   \"name\":\"" << (m_mapName.empty() ? "default" : m_mapName) << "\",\n"; // map name
        
        file << "   \"dimensions\":{\n"
        << "        \"sq_side\":\"" << m_squareSide << "\",\n"
        << "        \"sq_v\":\"" << m_squaresCount_vertical << "\",\n" // vertical squares
        << "        \"sq_h\":\"" << m_squaresCount_horizontal << "\",\n" // horizontal squares
        << "    },\n";
        
        file << "   \"colors\":{\n"
        << "        \"bg\":{\n" // background
        << "            \"r\":\"" << (int)m_backgroundColor.r << "\",\n" // red
        << "            \"g\":\"" << (int)m_backgroundColor.g << "\",\n" // green
        << "            \"b\":\"" << (int)m_backgroundColor.b << "\"\n" // blue
        << "        },\n"
        << "        \"gl\":{\n" // gridlines
        << "            \"r\":\"" << (int)m_gridLinesColor.r << "\",\n" // red
        << "            \"g\":\"" << (int)m_gridLinesColor.g << "\",\n" // green
        << "            \"b\":\"" << (int)m_gridLinesColor.b << "\"\n" // blue
        << "        },\n"
        << "        \"sqf\":{\n" // squarefill
        << "            \"r\":\"" << (int)m_squareFillColor.r << "\",\n" // red
        << "            \"g\":\"" << (int)m_squareFillColor.g << "\",\n" // green
        << "            \"b\":\"" << (int)m_squareFillColor.b << "\"\n" // blue
        << "        },\n"
        << "        \"sqc\":{\n" // squarecharge
        << "            \"r\":\"" << (int)m_squareChargedColor.r << "\",\n" // red
        << "            \"g\":\"" << (int)m_squareChargedColor.g << "\",\n" // green
        << "            \"b\":\"" << (int)m_squareChargedColor.b << "\"\n" // blue
        << "        },\n"
        << "        \"sqd\":{\n" // squaredischarge
        << "            \"r\":\"" << (int)m_squareDischargingColor.r << "\",\n" // red
        << "            \"g\":\"" << (int)m_squareDischargingColor.g << "\",\n" // green
        << "            \"b\":\"" << (int)m_squareDischargingColor.b << "\"\n" // blue
        << "        }\n"
        << "    },\n";
        
        file << "   \"squares\":[\n";
        bool firstCommaIgnored = false;
        for (auto itr : *m_pSquares_all)
        {
            if (firstCommaIgnored)
                file << ",\n";
            else
                firstCommaIgnored = true;
            
            file << "       {\n"
            << "            \"i\":\"" << itr->m_index << "\",\n" // index
            << "            \"f\":\"" << itr->m_fillState << "\",\n" // fillstate
            << "            \"c\":\"" << itr->m_chargeState << "\"\n" // chargestate
            << "        }"<< std::flush;
        }
        file << "\n";
        file << "   ]\n";
        file << "}\n";
        file.close();
    }
    else {
        std::cout << "Unable to open file\n";
    }
    
    FILE *p_file = NULL;
    p_file = fopen(fullPath.c_str(),"rb");
    fseek(p_file,0,SEEK_END);
    int size = ftell(p_file);
    fclose(p_file);
    std::cout << "file size: " << size << "\n";
    
    printFileAtLocation(fullPath);
}



/*void WyrezMap::writeToFile()
 {
 // Lambdas //
 auto getProgress = [](uint opTot, uint& opRem) -> float {
 opRem--;
 return 100 - opRem/(float)opTot*100;
 };
 
 auto addToDictFunc = [](CCDictionary* pDict, int val, std::string keyStr) -> void {
 pDict->setObject(CCString::create(std::to_string(val)), keyStr.c_str());
 };
 
 
 // Body //
 bool hasDelegate = (m_pDelegate != nullptr);
 
 uint operations_total = (uint)(8 + m_pSquares_all->size());
 uint opRem = operations_total;
 uint* pOperations_remaining = &opRem;
 if (hasDelegate) {
 m_pDelegate->updateWriteToFileProgress(100 - *pOperations_remaining/(float)operations_total*100);
 }
 
 std::string writablePath = CCFileUtils::sharedFileUtils()->getWritablePath();
 std::string fullPath = writablePath + "default.plist";
 
 CCDictionary *root = CCDictionary::create();
 
 CCString *mapName = CCString::create(m_mapName.empty() ? "default" : m_mapName);
 root->setObject(mapName, "string element key");
 if (hasDelegate) {
 m_pDelegate->updateWriteToFileProgress(getProgress(operations_total, *pOperations_remaining));
 }
 
 CCDictionary *dimensionsDict = CCDictionary::create();
 addToDictFunc(dimensionsDict, m_squareSide,                 "squareside");
 addToDictFunc(dimensionsDict, m_squaresCount_vertical,      "squaresx"); //vertical squares
 addToDictFunc(dimensionsDict, m_squaresCount_horizontal,    "squaresy"); //horizontal squares
 root->setObject(dimensionsDict, "dimensions");
 
 if (hasDelegate) {
 m_pDelegate->updateWriteToFileProgress(getProgress(operations_total, *pOperations_remaining));
 }
 
 CCDictionary *colorsDict = CCDictionary::create();
 {
 CCDictionary *colorDict_background = CCDictionary::create();
 addToDictFunc(colorDict_background, m_backgroundColor.r,    "r");
 addToDictFunc(colorDict_background, m_backgroundColor.g,    "g");
 addToDictFunc(colorDict_background, m_backgroundColor.b,    "b");
 colorsDict->setObject(colorDict_background, "background");
 }
 if (hasDelegate) {
 m_pDelegate->updateWriteToFileProgress(getProgress(operations_total, *pOperations_remaining));
 }
 {
 CCDictionary *colorDict_gridLines = CCDictionary::create();
 addToDictFunc(colorDict_gridLines, m_gridLinesColor.r,    "r");
 addToDictFunc(colorDict_gridLines, m_gridLinesColor.g,    "g");
 addToDictFunc(colorDict_gridLines, m_gridLinesColor.b,    "b");
 colorsDict->setObject(colorDict_gridLines, "gridlines");
 }
 if (hasDelegate) {
 m_pDelegate->updateWriteToFileProgress(getProgress(operations_total, *pOperations_remaining));
 }
 {
 CCDictionary *colorDict_squareFill = CCDictionary::create();
 addToDictFunc(colorDict_squareFill, m_squareFillColor.r,    "r");
 addToDictFunc(colorDict_squareFill, m_squareFillColor.g,    "g");
 addToDictFunc(colorDict_squareFill, m_squareFillColor.b,    "b");
 colorsDict->setObject(colorDict_squareFill, "squarefill");
 }
 if (hasDelegate) {
 m_pDelegate->updateWriteToFileProgress(getProgress(operations_total, *pOperations_remaining));
 }
 {
 CCDictionary *colorDict_squareCharge = CCDictionary::create();
 addToDictFunc(colorDict_squareCharge, m_squareChargedColor.r,    "r");
 addToDictFunc(colorDict_squareCharge, m_squareChargedColor.g,    "g");
 addToDictFunc(colorDict_squareCharge, m_squareChargedColor.b,    "b");
 colorsDict->setObject(colorDict_squareCharge, "squarecharged");
 }
 if (hasDelegate) {
 m_pDelegate->updateWriteToFileProgress(getProgress(operations_total, *pOperations_remaining));
 }
 {
 CCDictionary *colorDict_squareDischar = CCDictionary::create();
 addToDictFunc(colorDict_squareDischar, m_squareDischargingColor.r,    "r");
 addToDictFunc(colorDict_squareDischar, m_squareDischargingColor.g,    "g");
 addToDictFunc(colorDict_squareDischar, m_squareDischargingColor.b,    "b");
 colorsDict->setObject(colorDict_squareDischar, "squaredischarging");
 }
 root->setObject(colorsDict, "colors");
 if (hasDelegate) {
 m_pDelegate->updateWriteToFileProgress(getProgress(operations_total, *pOperations_remaining));
 }
 
 CCArray *squaresArray = CCArray::create();
 for (auto itr : *m_pSquares_all)
 {
 CCDictionary *squareDict = CCDictionary::create();
 //addToDictFunc(squareDict, itr->m_index,         "i"); // index
 //addToDictFunc(squareDict, itr->origin.x,        "x"); // x
 //addToDictFunc(squareDict, itr->origin.y,        "y"); // y
 addToDictFunc(squareDict, itr->m_fillState,     "f"); // fillstate
 addToDictFunc(squareDict, itr->m_chargeState,   "c"); // chargestate
 squaresArray->addObject(squareDict);
 
 if (hasDelegate) {
 m_pDelegate->updateWriteToFileProgress(getProgress(operations_total, *pOperations_remaining));
 }
 }
 root->setObject(squaresArray, "squares");
 
 if(root->writeToFile(fullPath.c_str()))
 CCLog("see the plist file at %s", fullPath.c_str());
 else
 CCLog("write plist file failed");
 
 if (hasDelegate) {
 m_pDelegate->updateWriteToFileProgress(getProgress(operations_total, *pOperations_remaining));
 }
 
 //this->printFileAtLocation(fullPath);
 }*/

