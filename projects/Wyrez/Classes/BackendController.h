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
    BackendController();
    virtual ~BackendController();
    void onHttpRequestCompleted(CCHttpClient *sender, CCHttpResponse *response);
    
public:
    static BackendController* getInstance();
    void signupUser(std::string name, std::string password, std::string email);
    void loginUser(std::string name, std::string password);
    void requestPasswordReset(std::string email);
    
};

#endif /* defined(__Wyrez__BackendController__) */
