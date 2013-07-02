//
//  ResourceProgressTimer.h
//  Wyrez
//
//  Created by stefan on 7/2/13.
//
//

#ifndef __Wyrez__ResourceProgressTimer__
#define __Wyrez__ResourceProgressTimer__

#include "cocos2d.h"

USING_NS_CC;

class ResourceProgressTimerDelegate
{
public:
    virtual ~ResourceProgressTimerDelegate() {};
    virtual void updateResourceHandler(int step) = 0;
    virtual void resourceHandlerFinished() = 0;
};

class ResourceProgressTimer : public CCProgressTimer
{
private:
    ResourceProgressTimerDelegate& m_rDelegate;
    
    CCSequence* m_sequence;
    int m_progress;
    int m_actionCount;
    int m_step;
    
private:
    void updateResource();
    void updateProgress();
    void finish();
    
public:
    ResourceProgressTimer() = delete;
    explicit ResourceProgressTimer(ResourceProgressTimerDelegate& rDelegate);
    virtual ~ResourceProgressTimer();
    static ResourceProgressTimer* create(CCSprite* pSprite, int steps, ResourceProgressTimerDelegate& rDelegate);
    virtual bool initWithSpriteAndArgs(CCSprite* pSprite, int steps);
    void start();
};

#endif /* defined(__Wyrez__ResourceProgressTimer__) */
