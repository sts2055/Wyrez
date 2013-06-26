//
//  BrushMenuItemSprite.h
//  Wyrez
//
//  Created by stefan on 6/26/13.
//
//

#ifndef __Wyrez__BrushMenuItemSprite__
#define __Wyrez__BrushMenuItemSprite__

#include "cocos2d.h"

class BrushMenuItemSpriteDelegate
{
public:
    virtual ~BrushMenuItemSpriteDelegate() {}
    virtual void brushMenuItemSpriteIsSelected() = 0;
    virtual void brushMenuItemSpriteIsUnselected() = 0;
};

class BrushMenuItemSprite : public cocos2d::CCMenuItemSprite
{
private:
    BrushMenuItemSpriteDelegate* m_pDelegate;
    
public:
    BrushMenuItemSprite();
    virtual ~BrushMenuItemSprite() {}
    
    virtual void selected();
    virtual void unselected();
    
    void setDelegate(BrushMenuItemSpriteDelegate* pDelegate) {m_pDelegate = pDelegate;}
};

#endif /* defined(__Wyrez__BrushMenuItemSprite__) */
