//
//  ResourceProgressTimer.cpp
//  Wyrez
//
//  Created by stefan on 7/2/13.
//
//

#include "ResourceProgressTimer.h"

ResourceProgressTimer::ResourceProgressTimer(ResourceProgressTimerDelegate& rDelegate)
: m_rDelegate(rDelegate)
, m_sequence(nullptr)
, m_progress(0)
, m_actionCount(0)
, m_step(0)
{
    
}

ResourceProgressTimer::~ResourceProgressTimer()
{
    CCLOGINFO("cocos2d: deallocating ResourceProgressTimer %p", this);
    CC_SAFE_RELEASE(m_sequence);
}

ResourceProgressTimer* ResourceProgressTimer::create(CCSprite* pSprite, int steps, ResourceProgressTimerDelegate& rDelegate)
{
    ResourceProgressTimer* pRet = new ResourceProgressTimer(rDelegate);
    if (pRet && pRet->initWithSpriteAndArgs(pSprite, steps))
    {
        pRet->autorelease();
    }
    else
    {
        CC_SAFE_DELETE(pRet);
    }
    return pRet;
}

bool ResourceProgressTimer::initWithSpriteAndArgs(CCSprite* pSprite, int steps)
{
    if (!CCProgressTimer::initWithSprite(pSprite)) {
        return false;
    }
    
    m_actionCount = steps;

    CCArray* actions = CCArray::create();
    for (int i=0; i< steps; i++)
    {
        auto updateResource = CCCallFunc::create(this, callfunc_selector(ResourceProgressTimer::updateResource));
        auto updateProgress = CCCallFunc::create(this, callfunc_selector(ResourceProgressTimer::updateProgress));
        actions->addObject(updateResource);
        actions->addObject(updateProgress);
    }
    auto delay = CCDelayTime::create(0.2);
    auto finish = CCCallFunc::create(this, callfunc_selector(ResourceProgressTimer::finish));
    actions->addObject(delay);
    actions->addObject(finish);
    
    m_sequence = CCSequence::create(actions);
    CC_SAFE_RETAIN(m_sequence);
    
    return true;
}

void ResourceProgressTimer::updateProgress()
{
    m_progress += 100/m_actionCount;
    this->setPercentage(m_progress);
}

void ResourceProgressTimer::updateResource()
{
    m_rDelegate.updateResourceHandler(++m_step);
}

void ResourceProgressTimer::start()
{
    CCAssert(m_sequence != nullptr, "ERROR: Sequence nullptr");
    CCAssert(this->getParent() != nullptr, "ERROR: Need to add to running scene");
    this->runAction(m_sequence);
}

void ResourceProgressTimer::finish()
{
    m_rDelegate.resourceHandlerFinished();
}