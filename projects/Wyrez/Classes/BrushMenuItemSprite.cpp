//
//  BrushMenuItemSprite.cpp
//  Wyrez
//
//  Created by stefan on 6/26/13.
//
//

#include "BrushMenuItemSprite.h"

BrushMenuItemSprite::BrushMenuItemSprite()
: m_pDelegate(nullptr)
{
    
}

void BrushMenuItemSprite::selected()
{
    CCMenuItemSprite::selected();
    
    if (m_pDelegate != nullptr) {
        m_pDelegate->brushMenuItemSpriteIsSelected();
    }
}

void BrushMenuItemSprite::unselected()
{
    CCMenuItemSprite::unselected();
    
    if (m_pDelegate != nullptr) {
        m_pDelegate->brushMenuItemSpriteIsUnselected();
    }
}