//
//  BackendController.h
//  Wyrez
//
//  Created by stefan on 7/3/13.
//
//

#ifndef __Wyrez__BackendController__
#define __Wyrez__BackendController__

#include "cocos2d.h"
#include "cocos-ext.h"

USING_NS_CC;
USING_NS_CC_EXT;

class BackendController : public CCObject
{
private:
    std::string* urlEncodeJSON(std::string json);
    
public:
    BackendController();
    virtual ~BackendController();
    static BackendController* getInstance();
    
    
    void test();
    void signupUser(std::string name, std::string password);
    void onHttpRequestCompleted(CCHttpClient *sender, CCHttpResponse *response);
};

#endif /* defined(__Wyrez__BackendController__) */
